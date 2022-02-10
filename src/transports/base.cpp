
#include <transports/base.h>


using namespace std;



acroTransport::acroTransport() {    
    status=-1;
    _State = std::make_shared< ConnectionStatus > ( ConnectionStatus::WAITING );
   _Type = std::make_shared< InstanceType > ( InstanceType::NOT_DEFINED );
}

acroTransport::acroTransport(string jOptions) {
    status=-1;   
   _State = std::make_shared< ConnectionStatus > ( ConnectionStatus::WAITING );
   _Type = std::make_shared< InstanceType > ( InstanceType::NOT_DEFINED );
    options = json::parse(jOptions);   
   _init(options);
}

void acroTransport::_init(json Options) {    
    
    if (Options.find("threads") != Options.end()) 
        threadsNum = Options["threads"].get<int>();
    if (Options.find("group") != Options.end()) 
        Group = Options["group"].get<int>();
}

bool acroTransport::listen(string URL)
{
    return false;
}

bool acroTransport::connect(string URL)
{            
    return false;
}

void acroTransport::drop(string sid)
{    
}


void acroTransport::_checkOptions() 
{    

}

ConnectionStatus acroTransport::Status() 
{    
    std::unique_lock<std::mutex> lktmp(_control);  //lktmp.unlock();    
    ConnectionStatus state = *_State;
    lktmp.unlock();    
    return state;
}
void acroTransport::Status(ConnectionStatus newState){
    std::unique_lock<std::mutex> lktmp(_control);  //lktmp.unlock();    
    *_State= newState;
    if (callbackOnStatus)
        callbackOnStatus(newState);    
    lktmp.unlock();    
}

InstanceType acroTransport::Type() 
{    
    std::unique_lock<std::mutex> lktmp(_control);  //lktmp.unlock();    
    InstanceType state = *_Type;
    lktmp.unlock();    
    return state;
}

void acroTransport::Type(InstanceType newState){
    std::unique_lock<std::mutex> lktmp(_control);  //lktmp.unlock();    
    *_Type = newState;
    lktmp.unlock();    
}

void acroTransport::onData(CallbackSocketVector callback) 
{        
    callbackData = callback;
}
void acroTransport::onConnection(CallbackConnection callback) 
{        
    callbackConnection = callback;
}

void acroTransport::onStatus(std::function<void(ConnectionStatus)> callback) 
{        
    callbackOnStatus = callback;
}

void acroTransport::onError(std::function<void(string)> callback) 
{        
    callbackOnError = callback;
}

bool acroTransport::send(char * buff, int size)
{    
    return send(buff, size, "");
}
bool acroTransport::send(char * buff, int size, string socket_id)
{    
    std::cout << "wrong way" << std::endl;   
    return false;
}

void acroTransport::close()
{        

}

void acroTransport::closePeer(string socket_id)
{        

}

