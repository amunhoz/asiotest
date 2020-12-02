"use strict"; 
const crypto = require('crypto');

const events = require("eventemitter3")

const socks = require("./socks/socks")
const sleep = m => new Promise(r => setTimeout(r, m))
const streamBackpressure = require("./stream")
const pcheck = require("./protocolcheck")
const PACKET_SIZE_LIMIT = 60*1024

module.exports = class proxyClient extends events{
    constructor(transport, options) {
        super()
        
        try {
            this.tranportClass = require("./transports/" + transport)
        } catch (e){
            throw new Error("Transport does not exists:" + transport)
        }

        if (!options) options = {}
        if (options.logfile) this.logfile = options.logfile
        this.maxRateLimit =  options.maxRateLimit ? options.maxRateLimit: 1 * 1024 * 1024 //1mb

        this.options = options

        if (options.maxRate) this.maxRate = options.maxRate
        this.reconnecting = false;
        this.cache = []
        this.conns = {}
        this.totals = {
            sended: 0,
            received:0
        }
        this.closed =false
    }

    async connect(address) {         
        this.transport = new this.tranportClass(this.options.transport)
        await this.transport.connect(address)
        var self = this
        this.check = new pcheck()
            

        this.transport.on("data", (data)=>{
            data = this.check.parse(data)
            self.handleData(Buffer.from(data))
        })
        return true
    }
    
    genId(){
        return new Promise(function(resolve, reject) {
            crypto.randomBytes(256, (err, buf) => {
                resolve(buf.readUInt32BE(0))                
            });
        })          
    }

    listen(port, address) {
        if (!address) address ="0.0.0.0"
        this.socks_server = socks.createServer({
            bindAddress: address,
            bindPort: port,
            username: "",
            password: ""
        })

        this.socks_server.listen(port, address,function(){
            //console.log('server on socks5://%s:%d', server.address().address,server.address().port);
        });
        this.socks_server.on("error",(err)=>{
            this.emit("error", err)            
        })
        this.socks_server.on('proxyConnection', this.tunnel.bind(this))
    }

    async tunnel(socket, info) {
        socket.pause()                
        socket.setTimeout(30000)
        socket.setNoDelay(true);
        socket.setKeepAlive(true);
        var id = await this.genId();        
        this.conns[id] = {
            socket:socket,
            host: info.host,
            port: info.port,
            cache: [],
            received: 0,
            sended: 0,
            pressure: false            
        }
        this.conns[id].startedAt = this._getUnixDate()
        this.conns[id].lastTrotle = this._getUnixDate()

        var self = this
        this.on("connect_" + id, (respo)=>{            
            info.onConnected();
            var ended = false            
            this.conns[id].stream = new streamBackpressure()
            this.conns[id].stream.pipe(socket)

            socket.on("data", (buff)=>{
                if (ended) return
                if (buff.length > PACKET_SIZE_LIMIT) {
                    this.sendData(id, buff.slice(0,PACKET_SIZE_LIMIT))
                    this.sendData(id, buff.slice(PACKET_SIZE_LIMIT))
                } else {
                    this.sendData(id, buff)                    
                }
                
                
                self._checkLimitRate(id);        
            })            
            var closeFunc = ()=>{
                if (ended) return                
                ended = true
                self._closeConnection(id, true)
            }
            socket.on("close", closeFunc)
            socket.on("timeout", closeFunc)
            socket.on("end", ()=>{
                if (ended) return
                self.sendCmd(id, { op: "end" })                        
            })
            socket.on("error", (e)=>{
                if (ended) return
                ended = true
                self._dropConnection(id, true)
            })
            socket.resume()
        })        
        this.sendCmd(id, {
            op: "connect",
            host: info.host,
            port: info.port
        })       
    }
    async _flushCache(id, force){       
        while(true) {
            let item = this.conns[id].cache.shift()
            if (!item) break;
            let ok = this.conns[id].socket.write(item);        
            if (ok ==false ){
                 return                             
            } 
        }                                        
        if (this.conns[id]) this.conns[id].waitDrain = false;       
    }

    _checkLimitRate(id){
        if (!this.maxRate) return
        if (!this.conns[id]) return
        if (this.conns[id].isPaused) return
        if (this.conns[id].sended <  this.maxRateLimit ) return

        let now = this._getUnixDate()
        if (now - this.conns[id].lastTrotle < 1 ) return        

        this.conns[id].lastTrotle = now            
        let ratedown = (this.conns[id].sended / (now - this.conns[id].startedAt))
        if (ratedown > this.maxRate) {            
            this.conns[id].isPaused = true
            this.conns[id].socket.pause();                
            let diff = (ratedown / this.maxRate)  * 1500            
            setTimeout(()=>{
                if (!this.conns[id]) return
                this.conns[id].socket.resume();
                this.conns[id].isPaused = false
            }, diff)
        }
        
    }
    
    _getUnixDate(){
        return Date.now()/1000   
    }

    _getCmdData(cmd) {
        let datacmd = Buffer.from(JSON.stringify(cmd))        
        let dataheader = Buffer.alloc(1)
        dataheader.writeUInt8(1)
        return Buffer.concat([dataheader,datacmd])
    }
    sendCmd(id, cmd) {
        cmd.id = id
        let data = this._getCmdData(cmd)
        this._send(data);
    }

    sendData(id, buff) {    
        let dataheader = Buffer.alloc(1)
        dataheader.writeUInt8(2) //2 - data header
        let dataid = Buffer.alloc(4)
        dataid.writeUInt32BE(id)
        let data = Buffer.concat([dataheader, dataid, buff])
        this._send(data);
        if (this.conns[id]) this.conns[id].sended += buff.length;
        this.totals.sended += buff.length
    }

    _send(data){      
        
        data = this.check.build(data)                
        this.transport.send(data); 
    }

    handleData(buff) {
        var id = "#"
        try {
            let type = buff.readUInt8(0)
            if (type == 2) { //data
                id = buff.readUInt32BE(1)
                let data = buff.slice(5)
                this.totals.received += data.length  
                this._writeSocket(id, data)                                    
            } else if (type == 1) { //info
                let cmd = JSON.parse(buff.slice(1).toString())
                this.handleCmds(cmd)
            }
        } catch (e) {
            this._dropConnection(id,  true)        
            this.emit("error", e)
        }        
    }
    _writeSocket(id, data) {
        if (!this.conns[id]) return this.emit("error", new Error("No connection with id:" + id))
        this.conns[id].received += data.length                            
        this.conns[id].stream.input(data);                
    }    
    /*
    _writeSocket(id, data) {
        if (!this.conns[id]) return console.log("no data ", id)
        this.conns[id].received += data.length                            
        
        if (this.conns[id].waitDrain) {
            this.conns[id].cache.push(data)
        } else {
            let ok = this.conns[id].socket.write(data);        
            if (ok ===false) {
                if (this.conns[id]) this.conns[id].waitDrain = true;                                 
            }
        }
    }
    */

    handleCmds(cmd) {
        
         //diverse cmds
        if (cmd.op == "response") {
            return this.emit("response_" + cmd.id, cmd)                    
        } 
        
        if (!this.conns[cmd.id]) return         
        if (cmd.op == "connected") {
            this.conns[cmd.id].remoteIp = cmd.remoteIp
            this.emit("connect_" + cmd.id, cmd)            
        } else if (cmd.op == "close") {       
            let time = 100            
            setTimeout(()=>{
                this._closeConnection(cmd.id)
            }, time)
        } else if (cmd.op == "end") {            
            let time = 50              
            setTimeout(()=>{
                if (this.conns[cmd.id]) {
                    this.conns[cmd.id].socket.end();            
                }
                if (this.conns[cmd.id]) this.conns[cmd.id].ending = false
            }, time)            
        } else if (cmd.op == "error") {
            this._dropConnection(cmd.id)                                  
        }  else {
            this._dropConnection(cmd.id)          
        } 
    }

    async request(data, timeout) {
        if (!timeout) timeout = this.requestTimeout
        var id = await this.genId();   
        this.sendCmd(id, {
            op: "request",
            id: id,
            data: data
        })      
        var self = this
        return new Promise(function(resolve, reject) {
            var timer = setTimeout(()=>{
                reject(new Error("Request timeout"))
            }, timeout)
            self.on("response_" + id, (respo)=>{            
                clearInterval(timer)
                resolve(respo.data)
            });
        })                   
    }

    _log (id, status) {
        if (!this.conns[id]) return
        let objLog = {
            received: this.conns[id].received,
            sended: this.conns[id].sended,
            host: this.conns[id].host,
            port: this.conns[id].port,
            remoteIp: this.conns[id].remoteIp ?  this.conns[id].remoteIp : "",            
            status: status
        }
        this.emit("log", objLog)
    }

    _closeConnection(id, sendCmd) {        
        if (!this.conns[id]) return    
        if (sendCmd)  this.sendCmd(id, {op: "close"})        
        
        //if (this.conns[id].socket.close) this.conns[id].socket.close()
        //else if (this.conns[id].socket.destroy) this.conns[id].socket.destroy()
        
        this.conns[id].socket.destroy()      

        this._log(id, "close");
        this._removeConnection(id);
    }
    _dropConnection(id, sendCmd) {        
        if (!this.conns[id]) return    
        if (sendCmd)  this.sendCmd(id, {
            op: "error"
        })        
        this.conns[id].socket.destroy()              
        this._log(id, "error");
        this._removeConnection(id);
    }

    _removeConnection(id){
        if (!this.conns[id]) return
        if (this.conns[id].socket.close) this.conns[id].socket.close();
        //if (this.conns[id].stream) this.conns[id].stream._dispose();
        delete this.conns[id];        
    }

    close(){
        this.closed = true;
        this.socks_server.close();
        this.transport.close();
    }

}