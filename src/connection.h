#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <vector>

class TConnection{
protected:
	struct sockaddr_in SockAddr;
	int SockFD;
	bool Connect;
	std::vector<std::pair<int, std::string> > m_clients;
public:
	TConnection();
	virtual ~TConnection();
	
	void onDisconnect();
	void onExit();

	virtual void onConnect(int);
	virtual void onConnect(std::string,int){	};
	virtual void onListening(){	};
	virtual void onTalking(){	};
};

TConnection::TConnection(){
	this->SockFD = socket(AF_INET, SOCK_STREAM, 0);
	this->Connect = false;
}

void TConnection::onConnect(int _port){
	if(SockFD < 0){
		perror("Can not Create Socket!");
	}

	memset(&SockAddr, 0, sizeof(SockAddr));
	SockAddr.sin_family = AF_INET;    
    SockAddr.sin_addr.s_addr = INADDR_ANY;
    SockAddr.sin_port = _port;

	// Validate bind socket address
    if(bind(SockFD,(const struct sockaddr *)&SockAddr, sizeof(SockAddr)) < 0){
		perror("Error Bind Failed");
		close(SockFD);
		exit(1);
    }

    // Validate if the socket is listening
    if(listen(SockFD, 10) < 0){
		perror("Error listen failed");
		close(SockFD);
		exit(1);
    }

    Connect = true;
    std::cout << "Server Connected\n";
}

void TConnection::onExit(){
	close(SockFD);
}

void TConnection::onDisconnect(){
	Connect = false;
}

TConnection::~TConnection(){

}

#endif