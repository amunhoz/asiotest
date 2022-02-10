const NetTransport = require("../lib/yasio.js");
var address = "127.0.0.1:5558"
async function start() {
    let options = {
        type:"tcp",    
    }
    let server = new NetTransport(options)    
    await server.listen(address)  
    server.on("data", (data, socket)=>{
        console.log("socket:" + socket, Buffer.from(data).length)        
        server.send("d".repeat(1000), socket)
    })  
    server.on("connection", (id, status, info)=>{
        console.log("connection:", id, status, info)
    }) 
}

start().catch((e)=>{
    console.log(e)
})