
#include <transports/ysocket.h>
#include <utils/random.h>
#include <utils/time.h>
#include <utils/easybuffer.h>
#include "kcp/ikcp.h"

#include <node.h>
#include <v8.h> 

using namespace std;
using namespace yasio;
using namespace yasio::inet;

YasioTransport::YasioTransport(){    
    status=-1;    
    _init();
}
YasioTransport::YasioTransport(std::string jOptions):acroTransport(jOptions){
    status=-1;
    options = json::parse(jOptions);
    _init();
    //initialize encryption
}

void YasioTransport::_init() {    
   //initialize maps
    connections = std::make_shared<   std::map<std::string , KCPConnection>    >
                                    (
                                      std::initializer_list<std::map<std::string , KCPConnection>::value_type>{}
                                    );   
    _checkOptions();
    
}
void YasioTransport::_checkOptions() {           
    std::string opt_type;
    if (options.find("type") != options.end()) opt_type =  options["type"].get<string>();      
    if (opt_type =="kcp"){
        _serverType = YCK_KCP_SERVER;
        _clientType = YCK_KCP_CLIENT;
    } else if (opt_type =="tcp"){
        _serverType = YCK_TCP_SERVER;
        _clientType = YCK_TCP_CLIENT;
    }
}

bool YasioTransport::listen(std::string URL)
{            
    Type(InstanceType::SERVER);    

    size_t found = URL.find_first_of(":");
    std::string host= URL.substr(0,found);
    std::string port= URL.substr(found+1);
    uint16_t porti = static_cast<uint16_t>(atoi(port.c_str()));
    
    try {	                          
        io_hostent endpoints[] = {{host.c_str(), porti}};        
        _service=  new io_service(endpoints, 1);                
        _service->set_option(YOPT_C_MOD_FLAGS, 0, YCF_REUSEADDR, 0);
        _service->set_option(YOPT_C_LFBFD_PARAMS, 1, 65535, 0, 4, 0);
        _service->set_option(YOPT_S_DEFERRED_EVENT, 0); // disable event queue             
          
        _service->start([&, this](event_ptr ev) {
            switch (ev->kind())
            {            
            case YEK_PACKET:
                //data                                
                if (!ev->packet().empty()){
                    _onData(std::move(ev->packet()), _getId(ev));
                }                
                break;
            case YEK_CONNECT_RESPONSE:
                //new connection                
                setup_kcp_transfer(ev->transport());
                _newConnection(ev);                
                break;
            case YEK_CONNECTION_LOST:
                //losted connection                
                _dropConnection(_getId(ev));                
                break;
            }            
        });
        _service->open(0, _serverType);                        
        status.exchange(0);          
        Status(ConnectionStatus::ONLINE);            
        if(DEBUG_MODE) std::cerr << "KCP: listen success"<< std::endl;    
        return true;
	}
	catch( exception& e ) {
        fprintf(stderr, "(KCP) CANT LISTEN: %s\n",  e.what());  
        return false;
    }  
    if(DEBUG_MODE) std::cerr << "KCP: listen success"<< std::endl;          
}



bool YasioTransport::connect(std::string URL)
{            
    Type(InstanceType::CLIENT);    

    size_t found = URL.find_first_of(":");
    string host = URL.substr(0,found);
    std::string port= URL.substr(found+1);
    uint16_t porti = atoi(port.c_str());
    
    Host = host;
    Port = porti;
    Idn = gen_random_number(); 
    try {
        io_hostent endpoints[] = {{host.c_str(), porti}};
          _service =  new io_service(endpoints, 1);        
        _service->set_option(YOPT_S_CONNECT_TIMEOUT, 5);
        _service->set_option(YOPT_C_MOD_FLAGS, 0, YCF_REUSEADDR, 0);
        _service->set_option(YOPT_C_LFBFD_PARAMS, 1, 65535, 0, 4, 0);
        _service->set_option(YOPT_S_DEFERRED_EVENT, 0); // disable event queue           

        _service->start([&, this](event_ptr ev) {
            switch (ev->kind())
            {            
            case YEK_PACKET:
                //data                             
                if (!ev->packet().empty()){                                  
                    _onData(std::move(ev->packet()), "");
                }                
                break;
            case YEK_CONNECT_RESPONSE:           
                //new connection                
                _client = ev->transport();     
                setup_kcp_transfer(_client);
                status.exchange(0);          
                Status(ConnectionStatus::ONLINE);                     
                break;
            case YEK_CONNECTION_LOST:
                //losted connection
                status.exchange(2);          
                Status(ConnectionStatus::OFFLINE);                    
                break;
            }            
        });
                

        _service->open(0, _clientType);            

        return true;
    } catch( exception& e ) {
        fprintf(stderr, "(KCP) CANT CONNECT: %s\n",  e.what());          
        return false;
    }    
    if(DEBUG_MODE) std::cerr << "KCP: connect success"<< std::endl;          
    
}

void YasioTransport::setup_kcp_transfer(transport_handle_t handle)
{
    auto kcp_handle = static_cast<io_transport_kcp*>(handle)->internal_object();    
    
    int opt_nodelay=1, opt_interval=20, opt_resend =2, opt_nc=1;    
    if (options.find("nodelay") != options.end()) opt_nodelay =  options["nodelay"].get<int>();        
    if (options.find("interval") != options.end()) opt_interval =  options["interval"].get<int>();        
    if (options.find("resend") != options.end()) opt_resend =  options["resend"].get<int>();        
    if (options.find("nc") != options.end()) opt_nc =  options["nc"].get<int>();        
    ::ikcp_nodelay(kcp_handle, opt_nodelay, opt_interval, opt_resend, opt_nc);

    int opt_mtu=1400;
    if (options.find("mtu") != options.end()) opt_mtu =  options["mtu"].get<int>();   
    ::ikcp_setmtu(kcp_handle, opt_mtu);

    int opt_sndwnd=8192, opt_rcvwnd=8192;
    if (options.find("sndwnd") != options.end()) opt_sndwnd =  options["sndwnd"].get<int>();   
    if (options.find("rcvwnd") != options.end()) opt_sndwnd =  options["rcvwnd"].get<int>();       
    ::ikcp_wndsize(kcp_handle, opt_rcvwnd, 8192);
 
}



bool YasioTransport::send(char * buff, int size, std::string socket_id)
{            
    if (Status() != ConnectionStatus::ONLINE) return false;
    std::vector<char> data;
    data.insert(data.end(), buff, buff+size); 
    try {
         if (Type() == InstanceType::SERVER){                               
            //server
            if (socket_id !="" && socket_id.length() >0) {
                if (!connections->count(socket_id)){
                    fprintf(stderr, "(KCP) SERVER ERROR: Socket not found on send message!! %s\n", socket_id);                        
                    return false; // cant send
                } 
               _service->write(connections->at(socket_id).socket, std::move(data));                                                          
            } else {         
                fprintf(stderr, "(KCP) SERVER ERROR: Socket empty on send message!!\n");                        
                return false;                          
            } 
        } else {  
            _service->write(_client, std::move(data));                                                      
        }
    } catch( exception& e ) {                        
        fprintf(stderr, "(KCP) CANT SEND MESSAGE: %s\n",  e.what());  
        return false;
    }            
    return true;
}

void YasioTransport::closePeer(std::string socket_id)
{      
    if (Type() != InstanceType::SERVER) return;
    if (!connections->count(socket_id)) return;     
    _service->close( connections->at(socket_id).socket);
    
}

void YasioTransport::close()
{           
   if (Type() == InstanceType::SERVER) {
       _service->stop();
   } else {
       //_client->disconnect();
   }
   status.exchange(9);
}

void YasioTransport::_onData(std::vector<char> data, string id) {                   
    //_reviewMessage(id, data.data(), data.size());   
    if (Type() == InstanceType::SERVER){    
        //std::cerr << "KCP: SERVER message received" << std::endl;                                                       
        callbackData(id, std::move(data));                                     
    } else {        
        //std::cerr << "KCP: CLIENT message received" << std::endl;                                                       
        callbackData("",  std::move(data));         
    }    
}



void YasioTransport::_newConnection(event_ptr& ev){                     
    std::string id = _getId(ev);
    if (connections->count(id)) {      
        //std::cerr << "KCP: addnew DROP CONNECTION?" << std::endl;                 
        _dropConnection(id);
    }
    
    KCPConnection item;    
    //create item
    //---------------------------
    item.id = ev->source_id();    
    //item.ip = conn->remote_point.address().to_string();
    //item.port = conn->remote_point.port();
    //item.info["ip"] = item.ip ;                
    //item.info["port"] =item.port;
    item.lastActivity = getCurrentMs();   
    item.socket = ev->transport();                  
    //insert itens
    _control.lock();
        connections->insert(make_pair(id, std::move(item)));
    _control.unlock();      
}



void YasioTransport::_dropConnection(std::string sid) { 
    if (connections->count(sid)) {
        KCPConnection* item = &connections->at(sid);
        if (callbackConnection!=NULL) {                
            callbackConnection(sid, ConnectionState::DISCONNECTED, item->info);
        }    
        _service->close(item->socket);
    };   
    _control.lock();
        connections->erase(sid);    
    _control.unlock();  
}


std::string YasioTransport::_getId(event_ptr& ev) {   
    return to_string(ev->source_id());
}

