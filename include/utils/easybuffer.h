#pragma once

#ifndef ACROBATIC_UTILS_BUFFER
#define ACROBATIC_UTILS_BUFFER
#include <cstring>
#include <vector>
#include <stdint.h>
#include <limits.h>

    // TODO XXX Little-Endian/Big-Endian problem.
#define evppbswap_64(x)                          \
    ((((x) & 0xff00000000000000ull) >> 56)       \
     | (((x) & 0x00ff000000000000ull) >> 40)     \
     | (((x) & 0x0000ff0000000000ull) >> 24)     \
     | (((x) & 0x000000ff00000000ull) >> 8)      \
     | (((x) & 0x00000000ff000000ull) << 8)      \
     | (((x) & 0x0000000000ff0000ull) << 24)     \
     | (((x) & 0x000000000000ff00ull) << 40)     \
     | (((x) & 0x00000000000000ffull) << 56))

namespace nh {

	auto htons = [](unsigned short h)
	{
		return (unsigned short)
			   ( h << 8 & 0xFF00U |
			     h >> 8 & 0x00FFU );
	};

	auto htonl = [](unsigned int h)
	{
		return (unsigned int)
			   ( h << 24 & 0xFF000000U |
			     h << 8  & 0x00FF0000U |
			     h >> 8  & 0x0000FF00U |
			     h >> 24 & 0x000000FFU );
	};

	auto ntohs = htons;
	auto ntohl = htonl;

}


class BufferEasy {
    public:
        enum Position
        {
            END,
            BEGIN
        };
        //char * buff;
        std::vector<char>  * buff;        
        BufferEasy(std::vector<char>  * mybuff){          
            buff = mybuff;
        }                
        int readPointer=0;
        
        void reset(){
            readPointer=0;
        }

        void append( char * buf, int size, Position pos=Position::END) {            
            if (pos==Position::END) buff->insert(buff->end(), buf, buf+size);
            else        buff->insert(buff->begin(), buf, buf+size);             
        }

        void appendUint16(uint16_t num, Position pos=Position::END) {
            int pointer = 0;    
            int bsize = sizeof(uint16_t);
            char sizebuff[bsize];    
            memcpy(sizebuff, &num, bsize);        
            if (pos==Position::END) buff->insert(buff->end(), sizebuff, sizebuff+bsize);
            else        buff->insert(buff->begin(), sizebuff, sizebuff+bsize);
        }

        void appendUint32(uint32_t num, Position pos=Position::END) {
            int pointer = 0;    
            int bsize = sizeof(uint32_t);
            char sizebuff[bsize];    
            memcpy(sizebuff, &num, bsize);                                    
            if (pos==Position::END) buff->insert(buff->end(), sizebuff, sizebuff+bsize);
            else        buff->insert(buff->begin(), sizebuff, sizebuff+bsize);
        }

        int16_t  readInt16() {
             int16_t  be16 = 0;
             int bsize = sizeof be16;
             ::memcpy(&be16, data(), bsize);
             readPointer += bsize;
             return nh::ntohs(be16);
        }

        uint16_t readUint16() {
             uint16_t be16 = 0;
             int bsize = sizeof be16;
             ::memcpy(&be16, data(), bsize);
             readPointer += bsize;
             return be16;
        }

        int32_t  readInt32() {
            int32_t  be32 = 0;
            int bsize = sizeof be32;
            ::memcpy(&be32, data(), bsize);
            readPointer += bsize;
            return nh::ntohl(be32);            
        }
        
        uint32_t readUint32() {
            int32_t be32 = 0;
            int bsize = sizeof be32;
            ::memcpy(&be32, data(), bsize);
            readPointer += bsize;
            return be32;            
        }
        uint64_t readUint64() {
            uint64_t be64 = 0;
            int bsize = sizeof be64;
            ::memcpy(&be64, data(), bsize);
            readPointer += bsize;
            return be64;            
        }
        int64_t readInt64() {
            int64_t  be64 = 0;
            int bsize = sizeof be64;
            ::memcpy(&be64, data(), bsize);
            readPointer += bsize;
            return evppbswap_64(be64);            
        }

        int size() {
            return buff->size()-readPointer;
        }
        int length() {
            return buff->size()-readPointer;
        }        
        char * data() {
            return buff->data()+readPointer;
        }

    private:
        
        
};
#endif 