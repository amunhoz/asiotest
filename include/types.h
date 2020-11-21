#ifndef ACROBATIC_TYPES
#define ACROBATIC_TYPES

#include <string>
#include <iostream>

//JSON
#include <nlohmann/json.hpp>


using namespace std;
using json = nlohmann::json;

typedef struct {	
    string transport_id;
    string socket_id;	
    string name;    
    json info;
} Connection ;

typedef struct {	
    string name="";
    int current=0;
	vector<string> itens;        
} ConnectionNames ;

typedef struct {	
    int conn_id=0;
    int socket_id;
	string id;
    bool local = false;
    std::function<void(char *, int)> callback = NULL;    
} Subscription ;

typedef struct {	
    string socket;
    vector<char> buff;      
} Packet ;


typedef map<string, Subscription> SubcriptionList;              

struct msgHeader
{
    //serialized
    string cmd = "#"; 
    map<string, string> info;
    map<string, string> extra;   
};

enum InstanceType
    {
            NOT_DEFINED,
            SERVER,
            CLIENT                
    };

enum ConnectionStatus
        {
                WAITING,
                ONLINE,
                OFFLINE,
                ERROR,
                CLOSED
        };


typedef std::function<void(char *, int, msgHeader * header)> CallbackMsg;  
typedef std::function<void(Packet* pack, msgHeader * header)> CallbackMsgPack;  
typedef std::function<void(char *, int)> CallbackData;  


#endif //ACROBATIC_TYPES
