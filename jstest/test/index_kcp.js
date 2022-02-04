const NetTransport = require("../lib/yasio.js");
var address = "127.0.0.1:5557"
async function start() {
    let options = {
        nodelay:1,
        interval: 40,
        resend:1,
        nc: 1,
        mtu:25000, 
        sndwnd:8192,
        opt_rcvwnd:8192
    }
    let server = new NetTransport(options)    
    await server.listen(address)  
    server.on("data", (data, socket)=>{
        console.log("socket:" + socket, Buffer.from(data).length)        
        server.send("d".repeat(1000), socket)
    })  
 
    let client = new NetTransport(options)    
    await client.connect(address)  
    client.on("data", (data, socket)=>{
        console.log("client:" , Buffer.from(data).length)        
    })  
    setTimeout(async()=>{                
        client.send("a".repeat(10450))        
    },1000)
}

start().catch((e)=>{
    console.log(e)
})