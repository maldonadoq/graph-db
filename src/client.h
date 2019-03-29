#ifndef _CLIENT_H_
#define _CLIENT_H_

#include "connection.h"

class TClient: public TConnection{
public:
	TClient(): TConnection(){	};

	void onConnect(std::string, int);
	void onTalking();
};

void TClient::onConnect(std::string _ip, int _port){
    if(SockFD < 0){
        perror("Can not Create Socket!");
        exit(0);
    }

    memset(&SockAddr, 0, sizeof(SockAddr));
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = _port;
    SockAddr.sin_addr.s_addr = inet_addr(_ip.c_str());
 
    if (connect(SockFD, (const struct sockaddr *)&SockAddr, sizeof(SockAddr)) < 0){
        perror("Connect failed");        
        exit(1);
    }

    Connect = true;
    std::cout << "Client Connected\n";
}

void TClient::onTalking(){
	// do this with the protocol!! 
	int buffer_size = 256;
    char buffer[buffer_size];
    int ret;

    std::cout << "Talking\n";
    std::string text = "";
    memset(&buffer, 0, buffer_size);
    while(Connect){
    	std::cout << "[Client]: ";
        getline(std::cin, text);
                
        ret = sendto(SockFD, text.c_str(), text.size(), 0, (struct sockaddr *) &SockAddr, sizeof(SockAddr));
        if (ret < 0){
			perror("Error Sending Data to Server Socket");
		}

        ret = recvfrom(SockFD, buffer, buffer_size, 0, NULL, NULL);
        if (ret < 0){
            perror("Error Receiving Data from Server Socket");
        }
        else{
            printf(" |%s\n", buffer);
        }        
    }
    onExit();
}

#endif