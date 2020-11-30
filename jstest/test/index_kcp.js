const Server = require("../lib/server.js");
const Client = require("../lib/client.js")
var address = "127.0.0.1:5557"
async function start() {

    let server = new Server("kcp", {        
        //maxRate: 5*1024*1024,        
        acl: [
            {route: "127.0.0*", action:"block"},
            //{direction: "out", route: "www.google.com*", action:"block"}
        ],
        transport:{
            kcp_nodelay: true,
            kcp_interval: 10,
            kcp_resend:2,
            kcp_congestion: false,
        }
    })    
    await server.listen(address)  
    server.on("request", (data, reply)=>{
        console.log(data)
        reply({hi:"there"})
    })  
 
    let client = new Client("kcp",{               
        transport:{        
            kcp_nodelay: true,
            kcp_interval: 10,
            kcp_resend:2,
            kcp_congestion: false,
        }
    })    
    await client.connect(address)  
    await client.listen(3129)
    
    client.on("log",(info) =>{
        //console.log(info)
    })

    setTimeout(async()=>{
        
        console.log("go test")
        let test = await client.request({cmd:"route"}, 30*1000)
        //let test = await clientx.request({cmd:"dns", domain:"verifact.com.br", ips:['172.67.183.223']}, 30*1000)
        //let ips = getIps(test.dns, "ipv4")
        console.log(test)
    },2000)
}

start().catch((e)=>{
    console.log(e)
})