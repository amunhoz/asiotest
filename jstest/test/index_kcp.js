const NetTransport = require("../lib/yasio.js");
var address = "127.0.0.1:5558"
async function start() {
    let options = {
        type:"tcp",
        nodelay:1,
        interval: 40,
        resend:1,
        nc: 1,
        mtu:65000, //packet size
        sndwnd:8192,
        opt_rcvwnd:8192
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
 
    let client = new NetTransport(options)    
    await client.connect(address)  
    client.on("status", (status)=>{
        console.log(status)
    })      

    client.on("data", (data, socket)=>{
        console.log("client:" , Buffer.from(data).length)
    })  
    client.on("close", (data, socket)=>{
        console.log("client closed")
    })  
    setTimeout(async()=>{                
        client.send("a".repeat(1000))        
    },1000)
    setTimeout(async()=>{                
        client.close()
    },10000)
}

start().catch((e)=>{
    console.log(e)
})