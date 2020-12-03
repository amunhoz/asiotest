#ifndef NODETRANSPORT_H
#define NODETRANSPORT_H

#define NAPI_EXPERIMENTAL
#include <napi.h>

#include <cmath>
#include <transports/kcp.h>
#include <utils/random.h>


using namespace Napi;


typedef struct {	                                    
        string socket;      
        vector<char> buff;   
} packet;

class LibNodeTransport : public Napi::ObjectWrap<LibNodeTransport>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    LibNodeTransport(const Napi::CallbackInfo&);
    //~LibNodeTransport() override = default;


private:    
    
    static Napi::FunctionReference constructor;
    bool DEBUG_MODE =false;
    Napi::Value Listen(const Napi::CallbackInfo&);
    Napi::Value Connect(const Napi::CallbackInfo&);
    Napi::Value Send(const Napi::CallbackInfo&);
    void Close(const Napi::CallbackInfo&);    
    
    void OnData(const Napi::CallbackInfo& info);
    

    void OnError(const Napi::CallbackInfo& info);
    void OnStatus(const Napi::CallbackInfo& info);
    void OnConnection(const Napi::CallbackInfo& info);
    Napi::Value  GetData(const Napi::CallbackInfo& info);
    void OnDataInternal(std::string socket, std::vector<char> data);    

    Napi::Value Status(const Napi::CallbackInfo& info);
        
    void Encryption(const Napi::CallbackInfo&);
    

    void _loadEvents(string type);

    //Acrobatic socket;    
    //std::shared_ptr< acroTransport > socket;  
    acroTransport* socket;  

    //map<string, cBackInfo*> subzCallbacks;        
    std::shared_ptr<Napi::ThreadSafeFunction> data_Callback;
    std::shared_ptr<Napi::ThreadSafeFunction> err_Callback;    
    std::shared_ptr<Napi::ThreadSafeFunction> status_Callback;    
    std::shared_ptr<Napi::ThreadSafeFunction> connection_Callback;    
    std::shared_ptr<Napi::ThreadSafeFunction> clientstatus_Callback;  

    int cacheLimit = 300;
    int cacheExpire = 30;
    std::mutex _control;                     
    std::shared_ptr<queue<Packet>> cache;   
            
    int count=0;
    string v_config = "{}";
    string v_type = "kcp";
};

#endif

