"use strict";

Object.defineProperty(exports, "__esModule", {
  value: true
});
exports.numberToBuffer = 
function numberToBuffer(num, len = 2, byteOrder = BYTE_ORDER_BE) {
  if (len < 1) {
    throw Error('len must be greater than 0');
  }

  const buf = Buffer.alloc(len);

  if (byteOrder === BYTE_ORDER_BE) {
    buf.writeUIntBE(num, 0, len);
  } else {
    buf.writeUIntLE(num, 0, len);
  }

  return buf;
}
