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
	fd_set master;
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
	this->SockFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	this->Connect = false;
}

void TConnection::onConnect(int _port){
	if(-1 == SockFD){
		perror("Can not Create Socket!");
	}

	memset(&SockAddr, 0, sizeof(struct sockaddr_in));
	SockAddr.sin_family = AF_INET;    
    SockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    SockAddr.sin_port = htons(_port);

	// Validate bind socket address
    if(-1 == bind(SockFD,(const struct sockaddr *)&SockAddr, sizeof(struct sockaddr_in))){
		perror("Error Bind Failed");
		close(SockFD);
		exit(1);
    }

    // Validate if the socket is listening
    if(-1 == listen(SockFD, 10)){
		perror("Error listen failed");
		close(SockFD);
		exit(1);
    }

    Connect = true;
    std::cout << "Server Connected\n";
    m_clients.push_back(std::make_pair(SockFD, "127.0.0.1"));
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