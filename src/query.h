#ifndef _QUERY_H_
#define _QUERY_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include "read-write.h"

class TQuery{
private:
    int m_querySock;
    struct sockaddr_in m_queryAddr;
public:
    TQuery();
    void Connect(std::string, int);
    void Talking();
};

TQuery::TQuery(){
    this->m_querySock = socket(AF_INET, SOCK_STREAM, 0);
}

void TQuery::Connect(std::string _ip, int _port){
    if(m_querySock < 0){
        perror("Can not Create Socket!");
        exit(0);
    }

    memset(&m_queryAddr, 0, sizeof(m_queryAddr));
    m_queryAddr.sin_family = AF_INET;
    m_queryAddr.sin_port = htons(_port);
    // m_queryAddr.sin_addr.s_addr = inet_addr(_ip.c_str());
 
    int res = inet_pton(AF_INET, _ip.c_str(), &m_queryAddr.sin_addr);
 
    if (0 > res){
      perror("error: first parameter is not a valid address family");
      close(m_querySock);
      exit(EXIT_FAILURE);
    }
    else if (0 == res){
      perror("char string (second parameter does not contain valid ipaddress");
      close(m_querySock);
      exit(EXIT_FAILURE);
    }

    if (connect(m_querySock, (const struct sockaddr *)&m_queryAddr, sizeof(m_queryAddr)) < 0){
        perror("Connect failed"); 
        exit(1);
    }

    std::cout << "Query Connected\n";
}

void TQuery::Talking(){
    std::cout << "Query Listenig-Talking\n";

    std::thread tread(thread_qread,   m_querySock);
    std::thread twrite(thread_qwrite, m_querySock, 100);

    tread.join();
    twrite.join();

    close(m_querySock);
}

#endif