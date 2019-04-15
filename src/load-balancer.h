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

#include "client-info.h"
#include "mthr.h"

class TLoadBalancer{
private:
	static std::mutex m_cmutex;
	static std::mutex m_qmutex;

	static std::vector<TClientInfo> m_clients;
	static std::vector<TClientInfo> m_querys;

	struct sockaddr_in m_serverAddrClients;
	struct sockaddr_in m_serverAddrQuerys;

	static int m_serverSockClients;	
	static int m_serverSockQuerys;

	static void SendToAllClients(std::string, int);
	static void SendToAllQuerys(std::string, int);
	static int  FindClientIdx(TClientInfo *);	
	static int  FindQueryIdx(TClientInfo *);	
public:
	TLoadBalancer();
	~TLoadBalancer();

	void ClientPort(int);
	void QueryPort(int);

	static void HandleClient(TClientInfo *);
	static void HandleQuery(TClientInfo *);
	
	static void ListeningClients();
	static void ListeningQuerys();
};

std::vector<TClientInfo> TLoadBalancer::m_clients;
std::vector<TClientInfo> TLoadBalancer::m_querys;

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
    std::cout << "Server to Clients Created!\n";
    std::cout << "Server to Clients Listening!\n";
}

void TLoadBalancer::ListeningClients(){
	TClientInfo	*cli;
	TThread		*thr;

	socklen_t cli_size = sizeof(sockaddr_in);
	struct sockaddr_in m_clientAddr;

	while(true){
		cli = new TClientInfo();
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

void TLoadBalancer::HandleClient(TClientInfo *cli){

	char buffer[256];
	std::string text = "";

	int idx, n;

	TLoadBalancer::m_cmutex.lock();

		cli->SetId(TLoadBalancer::m_clients.size());
	
		std::cout << "client: " << cli->m_name << " connected\tid: " << cli->m_id << "\n";
		TLoadBalancer::m_clients.push_back(*cli);

	TLoadBalancer::m_cmutex.unlock();
	
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
			text = cli->m_name + ": " + std::string(buffer);
			// send(cli->m_sock, text.c_str(), text.size(), 0);
			TLoadBalancer::SendToAllClients(text, cli->m_sock);
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
    std::cout << "Server to Querys Created!\n";
    std::cout << "Server to Querys Listening!\n";
}

void TLoadBalancer::ListeningQuerys(){
	TClientInfo	*cli;
	TThread		*thr;

	socklen_t cli_size = sizeof(sockaddr_in);
	struct sockaddr_in m_queryAddr;

	while(true){
		cli = new TClientInfo();
		thr = new TThread();

		// block
		cli->m_sock = accept(m_serverSockQuerys, (struct sockaddr *) &m_queryAddr, &cli_size);

	    if(cli->m_sock < 0)
	        perror("Error on accept");
	    else{
	    	cli->SetName(inet_ntoa(m_queryAddr.sin_addr));
	        thr->Create(TLoadBalancer::HandleQuery, cli);
	    }
	}
}

void TLoadBalancer::HandleQuery(TClientInfo *cli){

	char buffer[256];
	std::string text = "";

	int idx, n;

	TLoadBalancer::m_qmutex.lock();

		cli->SetId(TLoadBalancer::m_querys.size());
	
		std::cout << "query: " << cli->m_name << " connected\tid: " << cli->m_id << "\n";
		TLoadBalancer::m_querys.push_back(*cli);

	TLoadBalancer::m_qmutex.unlock();
	
	while(true){
		memset(buffer, 0, sizeof(buffer));
		n = recv(cli->m_sock, buffer, sizeof(buffer), 0);

		if(n == 0){
			std::cout << cli->m_name << " disconneted\n";
			close(cli->m_sock);

			TLoadBalancer::m_qmutex.lock();
				idx = TLoadBalancer::FindQueryIdx(cli);
				TLoadBalancer::m_querys.erase(TLoadBalancer::m_querys.begin()+idx);
			TLoadBalancer::m_qmutex.unlock();
			break;
		}
		else if(n < 0){
			perror("error receiving text");
		}
		else{
			text = cli->m_name + ": " + std::string(buffer);
			// send(cli->m_sock, text.c_str(), text.size(), 0);
			TLoadBalancer::SendToAllQuerys(text, cli->m_sock);
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

int TLoadBalancer::FindClientIdx(TClientInfo *_cli){
	for(unsigned i=0; i<m_clients.size(); i++)
		if(TLoadBalancer::m_clients[i].m_id == _cli->m_id)
			return i;

	return 0;
}

int TLoadBalancer::FindQueryIdx(TClientInfo *_cli){
	for(unsigned i=0; i<m_querys.size(); i++)
		if(TLoadBalancer::m_querys[i].m_id == _cli->m_id)
			return i;

	return 0;
}

TLoadBalancer::~TLoadBalancer(){
	m_clients.clear();
}

#endif