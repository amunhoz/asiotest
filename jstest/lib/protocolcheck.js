const crc = require("node-crc16")

module.exports = class Protocolcheck {
  constructor() {
    this.sended = 0
    this.received =0
  }
  parse(chunk){              
    let num = chunk.readUInt32BE(0);
    if (this.received+1 != num) 
      console.log("wrong order", this.received, num)    
    this.received = num
    console.log("received",num)
    let hashp = chunk.slice(4,6).toString("hex")
  
    let part = chunk.slice(6)
    let hasho = crc.checkSum(part)
    if (hashp != hasho) 
      console.log("wrong checksum")
    return part
  }
  build(chunk){
    this.sended++
    let head = Buffer.alloc(6)
    head.writeUInt32BE(this.sended);
    let hash = crc.checkSum(chunk, {retType: 'array'});    
    head[4] = hash[0]
    head[5] = hash[1]    
    console.log("sended", this.sended, chunk.length)
    return Buffer.concat([head, chunk])
  }
   
}