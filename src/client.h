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
	if(-1 == SockFD){
		perror("Can not Create Socket!");
	}    

	memset(&SockAddr, 0, sizeof(struct sockaddr_in));
	SockAddr.sin_family = AF_INET;
    SockAddr.sin_port = htons(_port);

    // struct hostent* host = gethostbyname(_ip.c_str());
    // SockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    
    int res = inet_pton(AF_INET, _ip.c_str(), &SockAddr.sin_addr);

    if (0 > res){
        perror("Error: first parameter is not a valid address family");
        close(SockFD);
        exit(1);
    }
    else if (0 == res){
        perror("Char string (second parameter does not contain valid ipaddress");
        close(SockFD);
        exit(1);
    }
 
    if (-1 == connect(SockFD, (const struct sockaddr *)&SockAddr, sizeof(struct sockaddr_in))){
        perror("Connect failed");
        close(SockFD);
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
                
        ret = write(SockFD, text.c_str(), text.size()+1);
        if (ret < 0){
			perror("Error Writing to Server Socket");
		}

        ret = read(SockFD, buffer, buffer_size);
        if (ret < 0){
            perror("Error Reading to Server Socket");
        }
        printf(" |%s\n", buffer);
    }
    onExit();
}

#endif