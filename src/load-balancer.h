#ifndef _LOAD_BALANCER_H_
#define _LOAD_BALANCER_H_

#include <iostream>
#include <vector>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "data.h"
#include "mthread.h"

class TLoadBalancer{
private:
	static std::vector<TData> m_clients;	

	int m_serverSock, m_clientSock;
	struct sockaddr_in m_serverAddr, m_clientAddr;

	static void SendToAll(std::string);
	static int  FindClientIdx(TData *);	
public:
	TLoadBalancer();
	~TLoadBalancer();

	void Connect(int);
	void Listening();
	static void HandleClient(TData *);
};

std::vector<TData> TLoadBalancer::m_clients;

TLoadBalancer::TLoadBalancer(){
	// TThread::InitMutex();
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
	TData   *cli;
	// std::thread thr;
	TThread *thr;

	socklen_t cli_size = sizeof(sockaddr_in);

	std::cout << "Server Listening!\n";
	while(true){
		cli = new TData();
		thr = new TThread();

		// block
		cli->m_sock = accept(m_serverSock, (struct sockaddr *) &m_clientAddr, &cli_size);

	    if(cli->m_sock < 0)
	        perror("Error on accept");
	    else
	        thr->Create(TLoadBalancer::HandleClient, cli);
	}
}

void TLoadBalancer::HandleClient(TData *cli){
	// TData *cli = (TData  *)_args;

	char buffer[256-25];
	std::string text = "";

	int idx, n;

	TThread::LockMutex(cli->m_name);

		cli->SetId(TLoadBalancer::m_clients.size());
		cli->SetName("[client "+std::to_string(cli->m_id)+"]");
	
		std::cout << cli->m_name << " connected\tid: " << cli->m_id << "\n";
		TLoadBalancer::m_clients.push_back(*cli);

	TThread::UnlockMutex(cli->m_name);	
	
	while(true){
		memset(buffer, 0, sizeof(buffer));
		n = recv(cli->m_sock, buffer, sizeof(buffer), 0);

		if(n == 0){
			std::cout << cli->m_name << " disconneted\n";
			close(cli->m_sock);

			TThread::LockMutex(cli->m_name);
				idx = TLoadBalancer::FindClientIdx(cli);
				TLoadBalancer::m_clients.erase(TLoadBalancer::m_clients.begin()+idx);
			TThread::UnlockMutex(cli->m_name);
			break;
		}
		else if(n < 0){
			perror("error receiving text");
		}
		else{
			text = std::string(buffer);
			TLoadBalancer::SendToAll(text);
		}
	}

	// return NULL;
}

void TLoadBalancer::SendToAll(std::string text){
	TThread::LockMutex("'Send'");
		// std::cout << "\ntext sending: " << text << "\n";
		for(unsigned i=0; i<m_clients.size(); i++)
			send(TLoadBalancer::m_clients[i].m_sock, text.c_str(), text.size(), 0);
	TThread::UnlockMutex("'Send'");
}

int TLoadBalancer::FindClientIdx(TData *_cli){
	for(unsigned i=0; i<m_clients.size(); i++)
		if(TLoadBalancer::m_clients[i].m_id == _cli->m_id)
			return i;

	return 0;
}

TLoadBalancer::~TLoadBalancer(){
	m_clients.clear();
}

#endif