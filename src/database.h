#ifndef _DATABASE_H_
#define _DATABASE_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include "../db/db.h"
#include "read-write.h"

class TDatabase{
private:
	TDb *m_db;

    int m_dbSock;
    struct sockaddr_in m_dbAddr;
public:
    TDatabase(std::string);
    TDatabase();
    ~TDatabase();
    
    void Connect(std::string, int);
    void Talking();
};

TDatabase::TDatabase(){
    this->m_dbSock = socket(AF_INET, SOCK_STREAM, 0);
}

TDatabase::TDatabase(std::string _name){
    this->m_dbSock = socket(AF_INET, SOCK_STREAM, 0);
    this->m_db = new TDb(_name);
}

void TDatabase::Connect(std::string _ip, int _port){
    if(m_dbSock < 0){
        perror("Can not Create Socket!");
        exit(0);
    }

    memset(&m_dbAddr, 0, sizeof(m_dbAddr));
    m_dbAddr.sin_family = AF_INET;
    m_dbAddr.sin_port = htons(_port);
    // m_dbAddr.sin_addr.s_addr = inet_addr(_ip.c_str());
 
    int res = inet_pton(AF_INET, _ip.c_str(), &m_dbAddr.sin_addr);
 
    if (0 > res){
      perror("error: first parameter is not a valid address family");
      close(m_dbSock);
      exit(EXIT_FAILURE);
    }
    else if (0 == res){
      perror("char string (second parameter does not contain valid ipaddress");
      close(m_dbSock);
      exit(EXIT_FAILURE);
    }

    if (connect(m_dbSock, (const struct sockaddr *)&m_dbAddr, sizeof(m_dbAddr)) < 0){
        perror("Connect failed"); 
        exit(1);
    }

    std::cout << "Database Connected\n";
}

void TDatabase::Talking(){
    std::cout << "Database Listening\n";

    unsigned buffer_size = 256;
	char buffer[buffer_size];

	while(true){
		memset(&buffer, 0, buffer_size);
		if(recv(m_dbSock, buffer, buffer_size, 0) > 0){			
			std::cout << buffer << "\n";
			// text = "recibido";
		}
	}

    close(m_dbSock);
}

TDatabase::~TDatabase(){
	delete m_db;
}

#endif