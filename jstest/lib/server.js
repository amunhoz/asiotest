"use strict"; 
const dns = require('dns');
const net = require("net")

const events = require("eventemitter3")
const LRU = require("lru-cache")
const micromatch = require('micromatch');
const common = require("./common")
const pcheck = require("./protocolcheck")

//const speedometer = require('speedometer')

module.exports = class proxyServer extends events{
    constructor(transport, options) {
        super()
        try {
            this.tranportClass = require("./transports/" + transport)
        } catch (e){
            throw new Error("Transport does not exists:" + transport)
        }
        
        if (!options) options = {}
        if (options.logfile) this.logfile = options.logfile    
        this.timeoutConnections = options.timeoutConnections ? options.timeoutConnections: 60000        
        this.maxRateLimit =  options.maxRateLimit ? options.maxRateLimit: 30 * 1024 * 1024 //10mb
        //this.bufferSize =  options.bufferSize ? options.bufferSize: 16 * 1024
        if (options.maxRate) this.maxRate = options.maxRate
        if (options.acl) this._parseAcl(options.acl)
        this.options = options
        
        this.conns = {}
        this.checks = {}
        this.clients = {}        
        this.aclCache =  new LRU({
            max: 5000,
            maxAge: 4 * 60 * 60 * 1000 //12hs
        })
        
    }

    _parseAcl(config) {        
        this.routes = {}
        for (var i in config) {      
            let key = config[i].route
            this.routes[key]  = config[i]
        }        
    }
    
    async listen(address) {   
        this.transport = new this.tranportClass(this.options.transport)
        await this.transport.listen(address)
        var self = this
        this.transport.on("data", (data, id)=>{
            if (!this.checks[id] ) this.checks[id] = new pcheck()
            data = this.checks[id].parse(data)
            self.handleData(data, id)
        })

        this.checkTimer = setInterval(()=>{
            for (var i in this.conns) {
                if (common.isExpired( this.timeoutConnections , this.conns[i].lastActivity)) {
                    this._dropConnection(i, "close", true)                    
                }
            }
        },  this.timeoutConnections)

    }

    sendCmd(socketId, id, cmd) {
        cmd.id = id
        let datacmd = Buffer.from(JSON.stringify(cmd))        
        let dataheader = Buffer.alloc(1)
        dataheader.writeUInt8(1)
        let data = Buffer.concat([dataheader,datacmd])
        try {            
            this._send(socketId, data);
        } catch(e) {            
            this.emit("error", e, {socketId: socketId, id: id, cmd: cmd})            
        }
        
    }

    sendData(socketId, id, buff) {    
        let dataheader = Buffer.alloc(1)
        dataheader.writeUInt8(2) //2 - data header
        let dataid = Buffer.alloc(4)
        dataid.writeUInt32BE(id)
        let data = Buffer.concat([dataheader, dataid, buff])
        try {
            this._send(socketId, data);            
            if (this.conns[id]) {
                this.conns[id].lastActivity = common.now();
                this.conns[id].sended += buff.length;
            } 
        } catch(e) {
            this.emit("error", e, {socketId: socketId, id: id})
        }                
    }

    _send(socketId, data) {        
        try {            
            
            data = this.checks[socketId].build(data)            
           this.transport.send(data, socketId);            
        } catch(e) {
            this.emit("error", e, {socketId: socketId})
        }    
    }

    handleData(buff, socketId) {            
        try {
            let type = buff.readUInt8(0)
            if (type == 2) { //data
                let id = buff.readUInt32BE(1)
                let data = buff.slice(5)            
                if (this.conns[id]) {                    
                    this.conns[id].socket.write(data)
                    this.conns[id].received += data.length
                }                    
            } else if (type == 1) { //info
                let cmd = JSON.parse(buff.slice(1).toString())
                this.handleCmds(cmd, socketId)
            }
        } catch (e) {
            this.emit("error", e, {socketId: socketId})
        }
    }

    createConn(info, socketId) {
        var id = info.id
        this.conns[id] = {}
        this.conns[id].host = info.host
        this.conns[id].port = info.port
        this.conns[id].received = 0
        this.conns[id].sended = 0
        this.conns[id].startedAt = this._getUnixDate()
        this.conns[id].lastTrotle = this._getUnixDate()
        this.conns[id].socketId = socketId
        this.conns[id].remoteIp = info.remoteIp
        
        //this.conns[id].socket = turbo.connect(info.port, info.host);
        this.conns[id].socket = new net.Socket()        
        this.conns[id].socket.connect(info.port, info.host)    
        var self = this;
        return new Promise(function(resolve, reject) {
                   
            var ended = false         
            var finalized = false   
            self.conns[id].socket.on("connect",()=>{                      
                const sizelimit = 60*1024
                self.conns[id].socket.on("data", (buff)=>{
                    if (ended) return                    
                    if (buff.length > sizelimit) {
                        self.sendData(socketId, id, buff.slice(0,sizelimit))
                        self.sendData(socketId, id, buff.slice(sizelimit))
                    } else {
                        self.sendData(socketId, id, buff)
                    }
                    
                    self._checkLimitRate(id);                    
                })                 
                self.conns[id].socket.setNoDelay(true);
                self.conns[id].socket.setKeepAlive(true);
                self.conns[id].socket.setTimeout(15000)                     
                 //turbo  
                 /*                               
                 self.conns[id].socket.read(Buffer.alloc(12*1024), function onread (err, buff, read) {
                    if (err) return
                    if (ended) return
                    self.sendData(socketId, id, buff.slice(0, read))         
                    self.conns[id].socket.read(Buffer.alloc(12*1024), onread)                 
                })
                */
                var endFunc =  ()=>{
                    if (!self.conns[id]) return     
                    finalized = true     
                    self.sendCmd(self.conns[id].socketId, id, {op: "end"})                    
                }
                var closeFunc = ()=>{
                    if (ended) return
                    if (!finalized) {
                        ///endFunc()
                    }
                    ended = true                    
                    self._dropConnection(id, "close", true)                            
                }
                self.conns[id].socket.on("close", closeFunc)                                           
                self.conns[id].socket.on("timeout", closeFunc)                                           
                self.conns[id].socket.on("error", (err)=>{
                    if (ended) return
                    ended = true                    
                    self._dropConnection(id, "error", true)                            
                })     
                self.conns[id].socket.on("end", endFunc)            
                
                self.sendCmd(socketId, id, {op:"connected", remoteIp: self.conns[id].remoteIp} )
                resolve(true)
            })                            
            //you can deal with close event and socket error handle
            self.conns[id].socket.on("error", (e)=>{
                if (ended) return
                ended = true
                self._dropConnection(id, "error")            
                if(reject) reject(e)
            })        

            
        })                 
    }
    _getUnixDate(){
        return Date.now()/1000   
    }

    _checkLimitRate(id){        
        if (!this.maxRate) return
        if (!this.conns[id]) return
        if (this.conns[id].isPaused) return
        if (this.conns[id].sended <  this.maxRateLimit ) return

        let now = this._getUnixDate()
        //if (now - this.conns[id].lastTrotle < 1 ) return        

        this.conns[id].lastTrotle = now            
        let ratedown = (this.conns[id].sended / (now - this.conns[id].startedAt))
        if (ratedown > this.maxRate) {            
            this.conns[id].isPaused = true
            this.conns[id].socket.pause();                
            let diff = (ratedown / this.maxRate)  * 1500
            if (diff > 5000) diff = 5000
            setTimeout(()=>{
                if (!this.conns[id]) return
                this.conns[id].socket.resume();
                this.conns[id].isPaused = false
            }, diff)
        }
        
    }

    _evaluateAcl(cmd, from) {
        var self = this
        var dolog = function (status){    
            let objLog = Object.assign({}, logTpl)    
            objLog.host = cmd.host
            objLog.port = cmd.port
            objLog.from = from
            objLog.status = status                            
            return self.emit("log", objLog)
        }
        for (var i in this.routes) {      
            if (micromatch.isMatch(cmd.host, this.routes[i].route)) {
                if (this.routes[i].action == "block") {
                    this.sendCmd(cmd.id, {op: "block"})
                    dolog("block")
                    return  "N"   
                } else if (this.routes[i].action == "allow") {
                    return  "Y"                                       
                }    
            }            
        }    
        //allow if not found
        return "Y"       

    }

    async handleCmds(cmd, socketId) {
        try {             
            //diverse cmds            
            if (cmd.op == "request") {
                this.emit("request", cmd.data, (respo)=>{
                    this.sendCmd(socketId,cmd.id, {
                        op:"response",
                        id: cmd.id,                        
                        data: respo
                    })                    
                })     
                return
            }              
            if (cmd.op == "connect") {          
                var cached = this.aclCache.get(cmd.host)
                var canGo = cached ? cached.acl : ""
                
                if (!canGo) {    
                    var ipaddress = cmd.host                 
                    if (!net.isIP(cmd.host)){
                        ipaddress = await this._resolveIp(cmd.host);
                        if (ipaddress) {
                            let cmdi = Object.assign({}, cmd, {host: ipaddress})
                            canGo = this._evaluateAcl(cmdi, socketId);
                        }                        
                    }
                    var canGo = this._evaluateAcl(cmd, socketId);
                    cached = {ip:ipaddress, acl:canGo};
                    this.aclCache.set(cmd.host, cached);
                }                                
                if (canGo==="N") return            
                cmd.remoteIp = cached ? cached.ip : ""                
                return await this.createConn(cmd, socketId)            
            } 

            if (!this.conns[cmd.id]) return            
            if (cmd.op == "close") {        
                this._dropConnection(cmd.id, "close")            
            } else if (cmd.op == "end") {                    
                this.conns[cmd.id].socket.end()
            } else if (cmd.op == "error") {                    
                this._dropConnection(cmd.id, "error")            
            }     
        } catch(e) {
            this.emit("error", e)
        }
          
    }
    _resolveIp(host) {                
        var self = this
        return new Promise(function(resolve, reject) {
            dns.lookup(host, (err, address, family) => {
                if (err) return resolve("")                
                resolve(address)                
            });
        });        
    }
    _log (id, status) {
        if (!this.conns[id]) return
        let objLog = {
            startedAt: this.conns[id].startedAt,
            received: this.conns[id].received,
            sended: this.conns[id].sended,
            host: this.conns[id].host,
            port: this.conns[id].port,
            remoteIp: this.conns[id].remoteIp ?  this.conns[id].remoteIp : "",
            from: this.conns[id].socketId,
            status: status
        }
        this.emit("log", objLog)
    }

    _dropConnection(id, type, sendCmd) {
        if (!this.conns[id]) return        

        if (sendCmd) {            
            this.sendCmd(this.conns[id].socketId, id, {op: type})
        }

        if (this.conns[id].socket.close) 
            this.conns[id].socket.close()
        if (this.conns[id].socket.destroy) 
          this.conns[id].socket.destroy()        
        
        this._log(id, type);
        delete this.conns[id]     
    }

    close(){
        clearInterval(this.checkTimer)
        this.transport.close()
    }

}