const events = require("eventemitter3")
module.exports = class proxyServer extends events{
    constructor(){
        super()

    }
    async listen(address){        
        throw new Error("not implemented")
    }
    async connect(address){
        throw new Error("not implemented")
    }
    async send(data, socket){
        throw new Error("not implemented")
    }
    async close(){
        throw new Error("not implemented")
    }
}