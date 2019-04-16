#ifndef _QUERY_H_
#define _QUERY_H_

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <vector>

#include "socket-info.h"
#include "thread-u.h"

#include "read-write.h"

class TQuery{
private:    
    static int m_querySockC;
    static int m_querySockS;

    struct sockaddr_in m_queryAddrC;
    struct sockaddr_in m_queryAddrS;

    static std::mutex m_qmutex;
    static std::vector<TSocket> m_dbs;

    static int FindDbIdx(TSocket *);
    static int FindRandomSockDb();
public:
    TQuery();

    //
    void ConnectToBalancer(std::string, int);
    static void TalkingToBalancer();

    void CreateServerQuery(int);
    static void ListeningDb();
    static void HandleDb(TSocket *);
};

std::mutex TQuery::m_qmutex;
std::vector<TSocket> TQuery::m_dbs;

int TQuery::m_querySockC;
int TQuery::m_querySockS;

TQuery::TQuery(){
    this->m_querySockC = socket(AF_INET, SOCK_STREAM, 0);
    this->m_querySockS = socket(AF_INET, SOCK_STREAM, 0);
}

void TQuery::ConnectToBalancer(std::string _ip, int _port){
    if(m_querySockC < 0){
        perror("Can not Create Socket!");
        exit(0);
    }

    memset(&m_queryAddrC, 0, sizeof(m_queryAddrC));
    m_queryAddrC.sin_family = AF_INET;
    m_queryAddrC.sin_port = htons(_port);
    // m_queryAddrC.sin_addr.s_addr = inet_addr(_ip.c_str());
 
    int res = inet_pton(AF_INET, _ip.c_str(), &m_queryAddrC.sin_addr);
 
    if (0 > res){
      perror("error: first parameter is not a valid address family");
      close(m_querySockC);
      exit(EXIT_FAILURE);
    }
    else if (0 == res){
      perror("char string (second parameter does not contain valid ipaddress");
      close(m_querySockC);
      exit(EXIT_FAILURE);
    }

    if (connect(m_querySockC, (const struct sockaddr *)&m_queryAddrC, sizeof(m_queryAddrC)) < 0){
        perror("Connect failed"); 
        exit(1);
    }
}

void TQuery::TalkingToBalancer(){
    std::cout << "Query Listenig-Talking\n";

    // std::thread tread(thread_qread,   m_querySockC, m_querySockS);
    std::thread twrite(thread_qwrite, m_querySockC, 100);

    // tread.join();    
    unsigned buffer_size = 256;
    char buffer[buffer_size];

    int sockdb;
    while(true){
        memset(&buffer, 0, buffer_size);        
        if(recv(m_querySockC, buffer, buffer_size, 0) > 0){          
            std::cout << buffer << "\n";
            sockdb = FindRandomSockDb();
            if(sockdb != -1)
                send(sockdb, buffer, buffer_size, 0);
        }
    }

    twrite.join();

    close(m_querySockC);
}


void TQuery::CreateServerQuery(int _port){
    int reuse = 1;  
    memset(&m_queryAddrS, 0, sizeof(sockaddr_in));

    m_queryAddrS.sin_family = AF_INET;
    m_queryAddrS.sin_addr.s_addr = INADDR_ANY;
    m_queryAddrS.sin_port = htons(_port);

    //Avoid bind error if the socket was not close()
    setsockopt(m_querySockS,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));

    if(bind(m_querySockS, (struct sockaddr *) &m_queryAddrS, sizeof(sockaddr_in)) < 0)
        perror("Failed to bind");

    listen(m_querySockS, 5);
    std::cout << "Server to Clients Created! [Listening]\n";
}

void TQuery::ListeningDb(){
    TSocket *qu;
    TThread *thr;

    socklen_t cli_size = sizeof(sockaddr_in);
    struct sockaddr_in m_dbclientAddr;

    while(true){
        qu  = new TSocket();
        thr = new TThread();

        // block
        qu->m_sock = accept(m_querySockS, (struct sockaddr *) &m_dbclientAddr, &cli_size);

        if(qu->m_sock < 0)
            perror("Error on accept");
        else{
            qu->SetName(inet_ntoa(m_dbclientAddr.sin_addr));
            thr->Create(TQuery::HandleDb, qu);
        }
    }
}

void TQuery::HandleDb(TSocket *qu){

    char buffer[256];
    std::string text = "";

    int idx, n;

    TQuery::m_qmutex.lock();

        qu->SetId(TQuery::m_dbs.size());
        std::cout << "db: " << qu->m_name << " connected\tid: " << qu->m_id << "\n";
        TQuery::m_dbs.push_back(*qu);

    TQuery::m_qmutex.unlock();
    
    while(true){
        memset(buffer, 0, sizeof(buffer));
        n = recv(qu->m_sock, buffer, sizeof(buffer), 0);

        if(n == 0){
            std::cout << qu->m_name << " disconneted\n";
            close(qu->m_sock);

            TQuery::m_qmutex.lock();
                idx = TQuery::FindDbIdx(qu);
                TQuery::m_dbs.erase(TQuery::m_dbs.begin()+idx);
            TQuery::m_qmutex.unlock();
            break;
        }
        else if(n < 0){
            perror("error receiving text");
        }
        else{           
            std::cout << "db: " << buffer << "\n";
        }
    }
}

int TQuery::FindDbIdx(TSocket *_q){
    for(unsigned i=0; i<m_dbs.size(); i++)
        if(TQuery::m_dbs[i].m_id == _q->m_id)
            return i;

    return 0;
}

int TQuery::FindRandomSockDb(){
    int tmp = m_dbs.size();
    if(tmp > 0)
        return m_dbs[rand()%tmp].m_sock;

    return -1;
}

#endif