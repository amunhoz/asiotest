#include "nodetransport.h"

using namespace Napi;
using namespace std::placeholders;

Napi::FunctionReference LibNodeTransport::constructor;

//constructor
//*********************************************************************
LibNodeTransport::LibNodeTransport(const Napi::CallbackInfo& info) : ObjectWrap(info) {
    Napi::Env env = info.Env();
    
    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
        return;
    }

    if (info[0] && !info[0].IsString()) {
        Napi::TypeError::New(env, "Transport type incorrect, use string 'nng', 'kcp'...")    
            .ThrowAsJavaScriptException();
    }
    v_type = info[0].As<Napi::String>().Utf8Value();
        
    if (info[1] && !info[1].IsString()) {
        Napi::TypeError::New(env, "Wrong config type")
          .ThrowAsJavaScriptException();
        return;
    } else if (info[1])  {
        v_config = info[1].As<Napi::String>().Utf8Value();
    }
      
}

//*********************************************************************
//*********************************************************************
// SOCKET CONNECTION
//*********************************************************************
//*********************************************************************

Napi::Value LibNodeTransport::Listen(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();    
    string address = info[0].As<Napi::String>().Utf8Value();    
    try {             
        if (v_type == "yasio") {                                                          
            socket = new YasioTransport(v_config);        
        } else {            
            Napi::TypeError::New(env, "No transport type found")
            .ThrowAsJavaScriptException();
            return Napi::Boolean::New(env, false);         
        }               
        bool isOnline = socket->listen(address);   
        if (!isOnline) return Napi::Boolean::New(env, false);
    } catch (const exception& e ) {        
        fprintf(stderr, "connect %s\n", e.what());        
        return Napi::Boolean::New(env, false);
    }           
    _loadEvents("listen");    
   

    return Napi::Boolean::New(env, true);
}


Napi::Value LibNodeTransport::Connect(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    bool result =false;    
    string address = info[0].As<Napi::String>().Utf8Value();    
    try {	                           
        if (v_type == "yasio") {                                                          
            socket = new YasioTransport (v_config);                
        }else {
            Napi::TypeError::New(env, "No transport type found")
            .ThrowAsJavaScriptException();
            return Napi::Boolean::New(env, false);        
        }                                                
        bool isOnline = socket->connect(address);   
        if (!isOnline) return Napi::Boolean::New(env, false);        
    } catch (const exception& e ) {
        std::cerr << "CONNECTERR" << std::endl;
        fprintf(stderr, "connect %s \n",  e.what());        
        return Napi::Boolean::New(env, false);
    }       
    _loadEvents("connect");
    return Napi::Boolean::New(env, true);

}

void LibNodeTransport::_loadEvents(string type) {

    socket->onError([this](string desc){
        if (!err_Callback) return;        
        err_Callback->call([this, desc](Napi::Env env, std::vector<napi_value> &args) {                
                args = {Napi::String::New(env, desc)};
        });  
    });

    socket->onStatus([this](ConnectionStatus state){
        if (!status_Callback) return;
        string statestr = "offline";
        if (state == ConnectionStatus::ONLINE) statestr = "online"; 
        status_Callback->call([this, statestr](Napi::Env env, std::vector<napi_value> &args) {                
                args = {Napi::String::New(env, statestr)};
        });          
    });    

    if (type == "listen") {
        socket->onConnection([this](string socket, ConnectionState state, json info){
            if (!status_Callback) return;
            status_Callback->call([this, socket, state, info](Napi::Env env, std::vector<napi_value> &args) { 
                string status = "offline";
                if (state== ConnectionState::CONNECTED ) status = "online";                 
                args = {Napi::String::New(env, socket), Napi::String::New(env, status), Napi::String::New(env, info.dump())};
            });                      
        });    
    }       
  
}

//*********************************************************************
//*********************************************************************
// send/receive
//*********************************************************************
//*********************************************************************
void LibNodeTransport::OnData(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();            
    data_Callback = std::make_unique<ThreadSafeCallback>(info[0].As<Napi::Function>());

    auto callBackCpp = [this](std::string socket, std::vector<char> data) {                            
        data_Callback->call([socket, data]( Napi::Env env, std::vector<napi_value> &args) mutable{              
            args = {Napi::Buffer<char>::Copy(env, data.data(), data.size()), Napi::String::New(env, socket)};                    
        });                 
    };             
    socket->onData(callBackCpp);    
    
    
}

Napi::Value LibNodeTransport::Send(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    bool result ;        
     if (info.Length() < 2) {
        Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
        return  Napi::Boolean::New(env, false);
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Destination code incorrect")
          .ThrowAsJavaScriptException();
        return  Napi::Boolean::New(env, false);
    }
     if (!info[1].IsBuffer()) {
        Napi::TypeError::New(env, "Buffer incorrect")
          .ThrowAsJavaScriptException();
        return  Napi::Boolean::New(env, false);
    }
    
    try {        
        string dest = info[0].As<Napi::String>().Utf8Value();    
        Napi::Buffer<char> buff = info[1].As<Napi::Buffer<char>>();         
        socket->send((char *)buff.Data(), (int) buff.Length(), dest);        
        result = true;
    } catch (const std::exception& e){
        fprintf(stderr, "%s\n", e.what());
        result = false;
    }    
    return Napi::Boolean::New(env, result);
}



//*********************************************************************
//*********************************************************************
// ENCRYPTION
//*********************************************************************
//*********************************************************************

//set encryption key
void LibNodeTransport::Encryption(const Napi::CallbackInfo& info) {
    //Napi::Env env = info.Env();
    //string key = info[0].As<Napi::String>().Utf8Value();    
    //dynamic_cast <acroEncryption*>(socket)->encryption(key);
}

//*********************************************************************
//*********************************************************************
// OTHER
//*********************************************************************
//*********************************************************************
void LibNodeTransport::OnError(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();    
    err_Callback = std::make_unique<ThreadSafeCallback>(info[0].As<Napi::Function>());    
}

void LibNodeTransport::OnStatus(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();        
    status_Callback = std::make_unique<ThreadSafeCallback>(info[0].As<Napi::Function>());    
}

void LibNodeTransport::OnConnection(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();        
    connection_Callback = std::make_unique<ThreadSafeCallback>(info[0].As<Napi::Function>());  
}

Napi::Value LibNodeTransport::Status(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();    
    string status = "offline";
    if (socket->Status() == ConnectionStatus::ONLINE ) {                                                        
            status = "online";               
    }                  
    return Napi::String::New(env, status);
}

void LibNodeTransport::Close(const Napi::CallbackInfo& info) {
    socket->close();
}

//init class
Napi::Object LibNodeTransport::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func =
      DefineClass(env,
                  "transport",
                  {
                        InstanceMethod("listen", &LibNodeTransport::Listen),
                        InstanceMethod("connect", &LibNodeTransport::Connect),
                        InstanceMethod("send", &LibNodeTransport::Send),
                        InstanceMethod("onData", &LibNodeTransport::OnData),

                        InstanceMethod("onError", &LibNodeTransport::OnError),
                        InstanceMethod("onStatus", &LibNodeTransport::OnStatus),
                        InstanceMethod("onConnection", &LibNodeTransport::OnConnection),
                        InstanceMethod("status", &LibNodeTransport::Status),
                        
                        InstanceMethod("encryption", &LibNodeTransport::Encryption),
                        InstanceMethod("close", &LibNodeTransport::Close),
                   });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();  
  exports.Set("transport", func);
  return exports;
}


