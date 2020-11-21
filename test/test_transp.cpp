#include <iostream>
#include <fstream>
#include <transports/kcp.h>
/*
*/
using namespace std;
void functest(std::string Error, char * buff, int size){
    //std::cout << buff << std::endl;     
}
int main()
{           
    KcpTransport server = KcpTransport();         
    server.listen("0.0.0.0:5315");
    int received =0;    
    string resp = "respo" + to_string(received);
    char * msgresp = (char*)resp.c_str();
    int sizeresp = resp.length();

    std::function<void(std::string, char *, int)> receiveMsgServer([&](std::string socket, char * buff, int size){        
        received++;
        if (received % 500 ==0) std::cout << "received " << received << std::endl;                         
        //std::cout << buff << std::endl;               
        std::this_thread::sleep_for(std::chrono::milliseconds(50)); //delay audio a little, just to sync        
        server.send( msgresp, sizeresp, socket, NULL); //respond        
    });  
    server.onData(receiveMsgServer);

    std::function<void(std::string, char *, int)> receiveMsgCli([&](std::string socket, char * buff, int size){        
          std::cout << "client received: " << buff << std::endl;               
    });  
    std::this_thread::sleep_for(std::chrono::milliseconds(5000)); //delay  a little
    KcpTransport client = KcpTransport();     
    client.connect("127.0.0.1:5315");
    client.onData(receiveMsgCli);    
  
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); //delay audio a little, just to sync        
    int sended =0;    
    std::thread nativeThread = std::thread( [&] {  
        //std::this_thread::sleep_for(std::chrono::milliseconds(5000)); //delay audio a little, just to sync        
        string send  = "dfasdfasfsadfdsadfdsafdasfdadfasdfasfsadfdsadfdsafdasfdadfasdfasfsadfdsadfdsafdasfdadfasdfasfsadfdsadfdsafdasfdadfasdfasfsadfdsadfdsafdasfdadfasdfasfsadfdsadfdsafdasfdadfasdfasfsadfdsadfdsafdasfdadfasdfasfsadfdsadfdsafdasfdadfasdfasfsadfdsadfdsafdasfdadfasdfasfsadfdsadfdsafdasfdadfasdfasfsadfdsadfdsafdasfdadfasdfasfsadfdsadfdsafdasfdadfasdfasfsadfdsadfdsafdasfdadfasdfasfsadfdsadfdsafdasfdas" ;
        char * msgsend = (char*)send.c_str();
        int sizesend = send.length();
        for(int i=0;i<1;++i) {
	        if (i % 1000 ==0) std::cout << "sended " << i << std::endl;                         
            std::this_thread::sleep_for(std::chrono::milliseconds(10)); //delay audio a little, just to sync        
            client.send(msgsend, sizesend, "", NULL);             
            //client2.send(msgsend, sizesend, "", NULL);                 
	    }   
    });
    nativeThread.detach();
    std::this_thread::sleep_for(std::chrono::milliseconds(10000000)); //delay audio a little, just to sync
    exit(1);
    
}