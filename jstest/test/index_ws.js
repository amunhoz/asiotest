const Server = require("../lib/server.js");
const Client = require("../lib/client.js")

async function start() {
     
    let server = new Server("websocket", {        
        //maxRate: 5*1024*1024,        
        acl: [
            {route: "127.0.0*", action:"block"},
            //{direction: "out", route: "www.google.com*", action:"block"}
        ]      
    })    
    await server.listen("ws://127.0.0.1:5556")  
    server.on("request", (data, reply)=>{
        console.log(data)
        reply({hi:"there"})
    })  
    let client = new Client("websocket",{ })    
    
    await client.connect("ws://127.0.0.1:5556")  
    await client.listen(3129)
    
    client.on("log",(info) =>{
        //console.log(info)
    })

    setTimeout(async()=>{
        
        console.log("go test")
        let test = await client.request({cmd:"route", domain:"verifact.com.br", ips:['']}, 30*1000)
        //let test = await clientx.request({cmd:"dns", domain:"verifact.com.br", ips:['172.67.183.223']}, 30*1000)
        //let ips = getIps(test.dns, "ipv4")
        console.log(test)
    },2000)
}

start().catch((e)=>{
    console.log(e)
})