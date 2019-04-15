#ifndef _LOAD_BALANCER_H_
#define _LOAD_BALANCER_H_

#include <iostream>
#include <vector>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <mutex>

#include "client-info.h"
#include "mthr.h"

class TLoadBalancer{
private:
	static std::mutex m_mutex;
	static std::vector<TClientInfo> m_clients;	

	int m_serverSock, m_clientSock;
	struct sockaddr_in m_serverAddr, m_clientAddr;

	static void SendToAll(std::string, int);
	static int  FindClientIdx(TClientInfo *);	
public:
	TLoadBalancer();
	~TLoadBalancer();

	void Connect(int);
	void Listening();
	static void HandleClient(TClientInfo *);
};

std::vector<TClientInfo> TLoadBalancer::m_clients;
std::mutex TLoadBalancer::m_mutex;

TLoadBalancer::TLoadBalancer(){
	m_serverSock = socket(AF_INET, SOCK_STREAM, 0);	
}


void TLoadBalancer::Connect(int _port){
	int reuse = 1;	
    memset(&m_serverAddr, 0, sizeof(sockaddr_in));

    m_serverAddr.sin_family = AF_INET;
    m_serverAddr.sin_addr.s_addr = INADDR_ANY;
    m_serverAddr.sin_port = htons(_port);

    //Avoid bind error if the socket was not close()
    setsockopt(m_serverSock,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));

    if(bind(m_serverSock, (struct sockaddr *) &m_serverAddr, sizeof(sockaddr_in)) < 0)
        perror("Failed to bind");

    listen(m_serverSock, 5);
    std::cout << "Server Created!\n";
}

void TLoadBalancer::Listening(){
	TClientInfo   *cli;
	TThread *thr;

	socklen_t cli_size = sizeof(sockaddr_in);

	std::cout << "Server Listening!\n";
	while(true){
		cli = new TClientInfo();
		thr = new TThread();

		// block
		cli->m_sock = accept(m_serverSock, (struct sockaddr *) &m_clientAddr, &cli_size);

	    if(cli->m_sock < 0)
	        perror("Error on accept");
	    else
	        thr->Create(TLoadBalancer::HandleClient, cli);
	}
}

void TLoadBalancer::HandleClient(TClientInfo *cli){

	char buffer[256-25];
	std::string text = "";

	int idx, n;

	TLoadBalancer::m_mutex.lock();

		cli->SetId(TLoadBalancer::m_clients.size());
		cli->SetName("[client "+std::to_string(cli->m_id)+"]");
	
		std::cout << cli->m_name << " connected\tid: " << cli->m_id << "\n";
		TLoadBalancer::m_clients.push_back(*cli);

	TLoadBalancer::m_mutex.unlock();
	
	while(true){
		memset(buffer, 0, sizeof(buffer));
		n = recv(cli->m_sock, buffer, sizeof(buffer), 0);

		if(n == 0){
			std::cout << cli->m_name << " disconneted\n";
			close(cli->m_sock);

			TLoadBalancer::m_mutex.lock();
				idx = TLoadBalancer::FindClientIdx(cli);
				TLoadBalancer::m_clients.erase(TLoadBalancer::m_clients.begin()+idx);
			TLoadBalancer::m_mutex.unlock();
			break;
		}
		else if(n < 0){
			perror("error receiving text");
		}
		else{
			text = cli->m_name + ": " + std::string(buffer);
			// send(cli->m_sock, text.c_str(), text.size(), 0);
			TLoadBalancer::SendToAll(text, cli->m_sock);
		}
	}
}

void TLoadBalancer::SendToAll(std::string _text, int _sock){
	TLoadBalancer::m_mutex.lock();
		// std::cout << "\ntext sending: " << text << "\n";
		for(unsigned i=0; i<m_clients.size(); i++){
			if(TLoadBalancer::m_clients[i].m_sock != _sock){
				send(TLoadBalancer::m_clients[i].m_sock, _text.c_str(), _text.size(), 0);
			}
		}
	TLoadBalancer::m_mutex.unlock();
}

int TLoadBalancer::FindClientIdx(TClientInfo *_cli){
	for(unsigned i=0; i<m_clients.size(); i++)
		if(TLoadBalancer::m_clients[i].m_id == _cli->m_id)
			return i;

	return 0;
}

TLoadBalancer::~TLoadBalancer(){
	m_clients.clear();
}

#endif