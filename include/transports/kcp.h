#ifndef ACROTRASNPORT_KCP2_INCLUDED
#define ACROTRASNPORT_KCP2_INCLUDED

#include <thread>
#include <set>
#include <future>

//local
#include <transports/base.h>

#include "yasio/yasio.hpp"
#include "yasio/ibstream.hpp"
#include "yasio/obstream.hpp"

using namespace std;
using namespace yasio::inet;


class KcpTransport : public acroTransport
{        
    protected:                
        typedef struct {	
            uint32_t  id;                                                    
            std::string ip;                                                    
            uint16_t port;                                                                
            json info;
            int lastActivity=0;
            transport_handle_t socket;
        } KCPConnection;
        std::shared_ptr<map<std::string , KCPConnection>> connections; 
        int _channel_kind = 26;
        io_service * _service;
        transport_handle_t _client;
               
        uint32_t  Idn;
        void _onData(std::vector<char> msg, std::string sid);    
        void _onEvent(event_ptr ev);                  
        
        void _newConnection(event_ptr& ev);          
        void _dropConnection(std::string sid);
        std::string _getId(event_ptr& ev);

        void _checkOptions();             
        void _init();             
        void setup_kcp_transfer(transport_handle_t handle);
          
        bool DEBUG_MODE = false;

        //client vars
        json infoConn;
        int lastActivity=0;
        //int _serverType = YCK_TCP_SERVER;
        //int _clientType = YCK_TCP_CLIENT;

        int _serverType = YCK_KCP_SERVER;
        int _clientType = YCK_KCP_CLIENT;
        
                       
        std::mutex _controlMsgs; 
                       
    private: 
        
      
    public:           
        string Host;
        int Port;
        KcpTransport();
        KcpTransport(std::string jOptions);
        bool listen(std::string URL);        
        bool connect(std::string URL);                 
        bool send(char * buff, int size, std::string socket_id, msgHeader * header);          
        void close(); 
        void closePeer(std::string socket_id);              
       
}; 


#endif 
