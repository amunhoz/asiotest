#ifndef ACROTRASNPORT_INCLUDED
#define ACROTRASNPORT_INCLUDED

#include <stdlib.h>
#include <vector>
#include <functional>
#include <thread>
#include <set>
#include <future>
#include <mutex>

//LOCAL
#include <types.h>


using namespace std;
using json = nlohmann::json;

enum ConnectionState
    {
            CONNECTED,
            DISCONNECTED
    };

typedef std::function<void(string, char *, int)> CallbackSocket; //socket_id, char, size
typedef std::function<void(string, ConnectionState, json)> CallbackConnection; //char, size, socket_id
typedef std::function<void(string, vector<char>)> CallbackSocketVector; //socket_id, char, size
class acroTransport 
{        
    protected:                               
        json options;
        CallbackSocketVector  callbackData = NULL;        
        CallbackConnection  callbackConnection = NULL;   
        std::function<void(string)> callbackOnError = NULL;
        std::function<void(ConnectionStatus)> callbackOnStatus = NULL;
        
        void _init(json Options);           
        virtual void _checkOptions(); 
        
        int threadsNum = 1;

        std::mutex _control; 
        std::shared_ptr<ConnectionStatus> _State;   
        std::shared_ptr<InstanceType> _Type;   

        std::atomic<int> status;    

    private: 
        

    public:           
        //functions
        acroTransport();
        acroTransport(string jOptions);
                
        string Id;
        string Group="";
        
        virtual bool listen(string URL);        
        
        virtual bool connect(string URL); 
        
        virtual void onData(CallbackSocketVector callback);
        virtual void onConnection(CallbackConnection callback);
        virtual void onStatus(std::function<void(ConnectionStatus)> callback);
        virtual void onError(std::function<void(string)> callback);
        
        virtual bool send(char * buff, int size);  
        virtual bool send(char * buff, int size, string socket_id);
        
        virtual void closePeer(string socket_id);
        virtual void close();         
        
        ConnectionStatus Status();
        void Status(ConnectionStatus newState);

        InstanceType Type();
        void Type(InstanceType newState);
        
        
        //InstanceType Type = InstanceType::NOT_DEFINED;         
        
        
        

}; 


#endif // ACROTRASNPORT_INCLUDED
