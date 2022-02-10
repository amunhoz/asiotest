#ifndef NODETRANSPORT_H
#define NODETRANSPORT_H

#define NAPI_EXPERIMENTAL
#include <napi.h>

#include <cmath>
#include <transports/ysocket.h>
#include <utils/random.h>
#include "thread-safe-callback.h"

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

    Napi::Value Status(const Napi::CallbackInfo& info);
        
    void Encryption(const Napi::CallbackInfo&);
    
    void Drop(const Napi::CallbackInfo& info);

    void _loadEvents(string type);

    //Acrobatic socket;        
    acroTransport* socket;  

    std::unique_ptr<ThreadSafeCallback> data_Callback  = nullptr;
    std::unique_ptr<ThreadSafeCallback> err_Callback  = nullptr;
    std::unique_ptr<ThreadSafeCallback> status_Callback  = nullptr;    
    std::unique_ptr<ThreadSafeCallback> connection_Callback  = nullptr;    
                        
    int count=0;
    string v_config = "{}";
    string v_type = "kcp";
};

#endif

