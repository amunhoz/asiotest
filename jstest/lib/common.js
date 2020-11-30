"use strict"; 
var funcs = {}
funcs.now = function() {
    return Date.now() 
}

funcs.isExpired = function(timeout, unixDate) {
    if ( funcs.now() > unixDate + (timeout))
        return true
    else 
        return false
}

module.exports = funcs