"use strict"; 

const base = require("./base")
const binding = require('../../../build/Release/transport.node');
module.exports = class AcroTransport extends base {
    constructor(options){
        super()
        if (!options) options = {}
        this.options = options
        this.instance = new binding.transport("kcp", "{}")
        var self = this
        
        this.instance.onError( (desc)=>{ // receive stream request response  
            self.emit("error", desc)              
        })
        this.instance.onStatus( (desc)=>{ // receive stream request response  
            self.emit("status", desc)              ;
        })
        this.instance.onConnection( (socket, state, info)=>{ // receive stream request response  
            self.emit("connection", socket, state, JSON.parse(info))              ;
        })         
    }
    async listen(address){
        this.type = "server"                
        let success =  this.instance.listen(address)
        if (!success) throw new Error("Cant connect to " + address)
        var self = this
        this.instance.onData((buff, socket)=>{ 
            //prepared for two methods, with or without getdata
            if (buff) return self.emit("data",Buffer.from(buff), socket)                     
            while(true) {
                let item = self.instance.getData()
                if (item==false) break;                
                self.emit("data",Buffer.from(item.data), item.socket)         
            }            
          })
          return true
    }
    async connect(address){
        this.type = "client"      
        let success = this.instance.connect(address)
        if (!success) throw new Error("Cant connect to " + address)
        var self = this
        this.instance.onData((buff, socket)=>{ //simple event            
             //prepared for two methods, with or without getdata
             if (buff) return self.emit("data",Buffer.from(buff), socket)                     
             while(true) {
                 let item = self.instance.getData()
                 if (item==false) break;                
                 let data = Buffer.from(item.data)                 
                 self.emit("data",data, item.socket)         
             }                                        
        })

        return true
    }
    send(data, socket) {
        if (!socket) socket = ""                
        this.instance.send(socket, Buffer.from(data))
    }
    close() {
        this.instance.close()        
    }
}