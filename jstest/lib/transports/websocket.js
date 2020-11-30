"use strict"; 
const uniqid = require('uniqid')
const base = require("./base")
const WebSocket = require('ws');

module.exports = class AcroTransport extends base {
    constructor(options){
        super()
        if (!options) options = {}
        this.options = options
        this.clients = {}
        this.online =  false
    }
    async listen(address){
        this.type = "server"
        let parts = address.split(":");
        let port = parts[parts.length-1]             
        this.options.port = parseInt(port)
        this.server = new WebSocket.Server(this.options)             
        this.server.binaryType = 'arraybuffer';
        var self = this
        this.server.on("connection", (socket)=>{
            socket.binaryType = 'arraybuffer';
            socket.id = uniqid()
            this.clients[socket.id] = socket
            socket.on("message", (data)=>{
                self.emit("data", Buffer.from(data), socket.id)                
            })
            socket.on("close", ()=>{
                delete this.clients[socket.id]              
            })                
        })       
        this.online = true
    }
    async connect(address){
        this.type = "client"
        this.address = address
        try {
            this.client = new WebSocket(address);
            this.client.binaryType = 'arraybuffer';                                        
        } catch(e) {
            return false
        }

        var self = this
        this.client.on('message', function incoming(data) {
            self.emit("data", Buffer.from(data))  
        });        
        this.client.on("close", ()=>{
            //reconnect
            if (this.closed) return
            this.online = false
            this._reconnect()
        }) 
        this.online = true
        return true          
    }
    _reconnect(){
        if (this.reconnecting == true) return
        let interval = Math.random() * 500        
        this.reconnecting = true
        var timer = setInterval(async ()=>{
            let success = await this.connect(this.address)
            if (!success) return
            this.reconnecting = false
            clearInterval(timer)
        }, interval)
    }

    send(data, socket) {
        if (!socket) socket = ""
        if (this.type == "client") {
            this.client.send(data)
        } else if (this.type == "server") {
            if (!this.clients[socket]) return
            this.clients[socket].send(data)
        }        
    }
    close() {
        this.closed = true
        if (this.type == "client") {                     
            this.client.close()            
        } else if (this.type == "server") {
            this.server.close()
        }        
    }
}