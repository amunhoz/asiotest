const { Readable } = require('stream');
//read stream do deal with backpressure
module.exports = class PressureStream extends Readable {
  constructor() {
    super()
    this.cache = []
    this.paused = false
    this.limit = 16 * 1024
  }
  input(chunk){              
    if (!this.paused) {
      this._flushcache()   
      this._feed(chunk)              
    } else {
      this.cache.push(chunk)
    }

  }
  _flushcache(){
    while (!this.paused ) {
      if (this.cache.length ==0) return;
      this._feed(this.cache.shift())
    }    
  }
   
  _read(size)
  {
    this.paused = false
    this._flushcache()
  } 
  
  _feed(data){
    let ok = this.push(data)
    if (!ok) this.paused = true
  }
}