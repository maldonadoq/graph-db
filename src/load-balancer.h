#ifndef _LOAD_BALANCER_H_
#define _LOAD_BALANCER_H_

#include <iostream>
#include <vector>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>

#include "socket-info.h"
#include "mthr.h"

class TLoadBalancer{
private:
	static std::mutex m_cmutex;
	static std::mutex m_qmutex;

	static std::vector<TSocket> m_clients;
	static std::vector<TSocket> m_querys;

	struct sockaddr_in m_serverAddrClients;
	struct sockaddr_in m_serverAddrQuerys;

	static int m_serverSockClients;	
	static int m_serverSockQuerys;

	static void SendToAllClients(std::string, int);
	static void SendToAllQuerys(std::string, int);
	static int  FindClientIdx(TSocket *);	
	static int  FindQueryIdx(TSocket *);
	static int 	FindQuerySockAvailable();
public:
	TLoadBalancer();
	~TLoadBalancer();

	void ClientPort(int);
	void QueryPort(int);

	static void HandleClient(TSocket *);
	static void HandleQuery(TSocket *);
	
	static void ListeningClients();
	static void ListeningQuerys();

	static void PrintClientSockets();
	static void PrintQuerySockets();
};

std::vector<TSocket> TLoadBalancer::m_clients;
std::vector<TSocket> TLoadBalancer::m_querys;

int TLoadBalancer::m_serverSockClients;
int TLoadBalancer::m_serverSockQuerys;

std::mutex TLoadBalancer::m_cmutex;
std::mutex TLoadBalancer::m_qmutex;

TLoadBalancer::TLoadBalancer(){
	m_serverSockClients = socket(AF_INET, SOCK_STREAM, 0);
	m_serverSockQuerys  = socket(AF_INET, SOCK_STREAM, 0);
}


void TLoadBalancer::ClientPort(int _port){
	int reuse = 1;	
    memset(&m_serverAddrClients, 0, sizeof(sockaddr_in));

    m_serverAddrClients.sin_family = AF_INET;
    m_serverAddrClients.sin_addr.s_addr = INADDR_ANY;
    m_serverAddrClients.sin_port = htons(_port);

    //Avoid bind error if the socket was not close()
    setsockopt(m_serverSockClients,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));

    if(bind(m_serverSockClients, (struct sockaddr *) &m_serverAddrClients, sizeof(sockaddr_in)) < 0)
        perror("Failed to bind");

    listen(m_serverSockClients, 5);
    std::cout << "Server to Clients Created! [Listening]\n";
}

void TLoadBalancer::ListeningClients(){
	TSocket	*cli;
	TThread		*thr;

	socklen_t cli_size = sizeof(sockaddr_in);
	struct sockaddr_in m_clientAddr;

	while(true){
		cli = new TSocket();
		thr = new TThread();

		// block
		cli->m_sock = accept(m_serverSockClients, (struct sockaddr *) &m_clientAddr, &cli_size);

	    if(cli->m_sock < 0)
	        perror("Error on accept");
	    else{
	    	cli->SetName(inet_ntoa(m_clientAddr.sin_addr));
	        thr->Create(TLoadBalancer::HandleClient, cli);
	    }
	}
}

void TLoadBalancer::HandleClient(TSocket *cli){

	char buffer[256];
	std::string text = "";

	int idx, n;

	TLoadBalancer::m_cmutex.lock();

		cli->SetId(TLoadBalancer::m_clients.size());
	
		std::cout << "client: " << cli->m_name << " connected\tid: " << cli->m_id << "\n";
		TLoadBalancer::m_clients.push_back(*cli);

		// PrintClientSockets();
	    // PrintQuerySockets();

	TLoadBalancer::m_cmutex.unlock();
	
	int sockquery;
	while(true){
		memset(buffer, 0, sizeof(buffer));
		n = recv(cli->m_sock, buffer, sizeof(buffer), 0);

		if(n == 0){
			std::cout << cli->m_name << " disconneted\n";
			close(cli->m_sock);

			TLoadBalancer::m_cmutex.lock();
				idx = TLoadBalancer::FindClientIdx(cli);
				TLoadBalancer::m_clients.erase(TLoadBalancer::m_clients.begin()+idx);
			TLoadBalancer::m_cmutex.unlock();
			break;
		}
		else if(n < 0){
			perror("error receiving text");
		}
		else{			
			sockquery = TLoadBalancer::FindQuerySockAvailable();

			if(sockquery > 0){
				text = cli->m_name + ": " + std::string(buffer);
				send(sockquery, text.c_str(), text.size(), 0);
			}

			// send(cli->m_sock, text.c_str(), text.size(), 0);
			// TLoadBalancer::SendToAllClients(text, cli->m_sock);
		}
	}
}

void TLoadBalancer::QueryPort(int _port){
	int reuse = 1;	
    memset(&m_serverAddrQuerys, 0, sizeof(sockaddr_in));

    m_serverAddrQuerys.sin_family = AF_INET;
    m_serverAddrQuerys.sin_addr.s_addr = INADDR_ANY;
    m_serverAddrQuerys.sin_port = htons(_port);

    //Avoid bind error if the socket was not close()
    setsockopt(m_serverSockQuerys,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));

    if(bind(m_serverSockQuerys, (struct sockaddr *) &m_serverAddrQuerys, sizeof(sockaddr_in)) < 0)
        perror("Failed to bind");

    listen(m_serverSockQuerys, 5);
    std::cout << "Server to Querys Created! [Listening]\n";
}

void TLoadBalancer::ListeningQuerys(){
	TSocket	*qu;
	TThread		*thr;

	socklen_t cli_size = sizeof(sockaddr_in);
	struct sockaddr_in m_queryAddr;

	while(true){
		qu = new TSocket();
		thr = new TThread();

		// block
		qu->m_sock = accept(m_serverSockQuerys, (struct sockaddr *) &m_queryAddr, &cli_size);

	    if(qu->m_sock < 0)
	        perror("Error on accept");
	    else{
	    	qu->SetName(inet_ntoa(m_queryAddr.sin_addr));
	        thr->Create(TLoadBalancer::HandleQuery, qu);
	    }
	}
}

void TLoadBalancer::HandleQuery(TSocket *qu){

	char buffer[2];
	std::string text = "";

	int idx, n;

	TLoadBalancer::m_qmutex.lock();

		qu->SetId(TLoadBalancer::m_querys.size());
	
		std::cout << "query: " << qu->m_name << " connected\tid: " << qu->m_id << "\n";
		TLoadBalancer::m_querys.push_back(*qu);

		// PrintClientSockets();
	    // PrintQuerySockets();

	TLoadBalancer::m_qmutex.unlock();
	
	while(true){
		memset(buffer, 0, sizeof(buffer));
		n = recv(qu->m_sock, buffer, sizeof(buffer), 0);

		if(n == 0){
			std::cout << qu->m_name << " disconneted\n";
			close(qu->m_sock);

			TLoadBalancer::m_qmutex.lock();
				idx = TLoadBalancer::FindQueryIdx(qu);
				TLoadBalancer::m_querys.erase(TLoadBalancer::m_querys.begin()+idx);
			TLoadBalancer::m_qmutex.unlock();
			break;
		}
		else if(n < 0){
			perror("error receiving text");
		}
		else{
			// std::cout << atof(buffer) << " ";
			idx = TLoadBalancer::FindQueryIdx(qu);
			TLoadBalancer::m_querys[idx].SetAvailable(atof(buffer));
			// qu->SetAvailable(atof(buffer));
		}
	}
}

void TLoadBalancer::SendToAllClients(std::string _text, int _sock){
	TLoadBalancer::m_cmutex.lock();
		for(unsigned i=0; i<m_clients.size(); i++){
			if(TLoadBalancer::m_clients[i].m_sock != _sock){
				send(TLoadBalancer::m_clients[i].m_sock, _text.c_str(), _text.size(), 0);
			}
		}
	TLoadBalancer::m_cmutex.unlock();
}

void TLoadBalancer::SendToAllQuerys(std::string _text, int _sock){
	TLoadBalancer::m_qmutex.lock();
		for(unsigned i=0; i<m_querys.size(); i++){
			if(TLoadBalancer::m_querys[i].m_sock != _sock){
				send(TLoadBalancer::m_querys[i].m_sock, _text.c_str(), _text.size(), 0);
			}
		}
	TLoadBalancer::m_qmutex.unlock();
}

int TLoadBalancer::FindClientIdx(TSocket *_cli){
	for(unsigned i=0; i<m_clients.size(); i++)
		if(TLoadBalancer::m_clients[i].m_id == _cli->m_id)
			return i;

	return 0;
}

int TLoadBalancer::FindQueryIdx(TSocket *_q){
	for(unsigned i=0; i<m_querys.size(); i++)
		if(TLoadBalancer::m_querys[i].m_id == _q->m_id)
			return i;

	return 0;
}

int TLoadBalancer::FindQuerySockAvailable(){
	
	if(TLoadBalancer::m_querys.size() > 0){
		float min = TLoadBalancer::m_querys[0].m_available;
		int sock = TLoadBalancer::m_querys[0].m_sock;

		for(unsigned i=1; i<TLoadBalancer::m_querys.size(); i++){
			if(TLoadBalancer::m_querys[i].m_available < min){
				min = TLoadBalancer::m_querys[i].m_available;
				sock = TLoadBalancer::m_querys[i].m_sock;
			}
		}

		return sock;
	}

	return -1;
}

void TLoadBalancer::PrintClientSockets(){
	std::cout << "Clients: ";
	for(unsigned i=0; i<TLoadBalancer::m_clients.size(); i++)	
		std::cout << TLoadBalancer::m_clients[i].m_sock << " ";

	std::cout << "\n";
}

void TLoadBalancer::PrintQuerySockets(){
	std::cout << "Querys:  ";
	for(unsigned i=0; i<TLoadBalancer::m_querys.size(); i++)	
		std::cout << TLoadBalancer::m_querys[i].m_sock << " ";

	std::cout << "\n";
}

TLoadBalancer::~TLoadBalancer(){
	m_clients.clear();
}

#endif