"use strict"; 
const events = require("eventemitter3")
const binding = require('../../build/Release/transport.node');
module.exports = class AcroTransport extends events {
    constructor(options){
        super()
        if (!options) options = {}
        this.options = options
        this.instance = new binding.transport("yasio",  JSON.stringify(options))        
        var self = this
        this.closed = false
        this.instance.onError( (desc)=>{ // receive stream request response  
            self.emit("error", desc)              
        })
        this.instance.onStatus( (desc)=>{ // receive stream request response  
            if (desc ==="offline") {
                this.close()
            } else if (desc ==="online"){
                this.emit("connected")
            }
            self.emit("status", desc)              ;
        })
        this.instance.onConnection( (socket, state, info)=>{ // receive stream request response  
            info  = JSON.parse(info||"")
            info = info || {}
            let addr = (info.addr || ":").split(":")
            info.ip = addr[0]
            info.port = addr[1]
            info.connected = false
            if (state == "online") info.connected = true
            self.emit("connection", socket, state, info)              ;
        })         
    }
    async listen(address){
        this.type = "server"                
        let success =  this.instance.listen(address)
        if (!success) throw new Error("Cant connect to " + address)
        var self = this
        this.instance.onData((buff, socket)=>{             
            if (!buff) return this.emit("error", new Error("Buffer not received from yasio"))
            try {
                self.emit("data",Buffer.from(buff), socket)     
            } catch (e){
                this.emit("error", e)
            }                                                                                        
          })
          return true
    }
    async connect(address){
        this.type = "client"      
        let success = this.instance.connect(address)
        if (!success) throw new Error("Cant connect to " + address)
        var self = this
        this.instance.onData((buff)=>{ //simple event                         
            if (!buff) return this.emit("error", new Error("Buffer not received from yasio"))
            try {
                self.emit("data",Buffer.from(buff))     
            } catch (e){
                this.emit("error", e)
            }                                                           
        })
        return true
    }
    send(data, socket) {
        if (!socket) socket = ""                    
        try {
            this.instance.send(socket, Buffer.from(data))
        } catch (e){
            this.emit("error", e)
        }
        
    }
    close() {
        if (this.closed) return
        this.closed = true
        this.emit("close")
        this.instance.close()        
    }
}