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
 
    let client = new NetTransport(options)    
    await client.connect(address)  
    client.on("status", (status)=>{
        console.log(status)
    })      

    client.on("data", (data, socket)=>{
        console.log("client:" , Buffer.from(data).length)        
    })  
    setInterval(async()=>{                
        client.send("a".repeat(1000))        
    },1000)
}

start().catch((e)=>{
    console.log(e)
})