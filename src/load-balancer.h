#ifndef _LOAD_BALANCER_H_
#define _LOAD_BALANCER_H_

#include "connection.h"
#include <queue>

class TLoadBalancer: public TConnection{
private:
	unsigned querys;
	float *availability;
	std::queue<std::string> m_queue;	
public:
	TLoadBalancer(unsigned _query): TConnection(){
		this->querys = _query;
		this->availability = new float[this->querys];
	}

	void onListening();
	void onUpdateQuery();
	unsigned getAvailable();
	int getMaxFD();
	void rmFD(int);
	void closeClients();
};

void TLoadBalancer::onUpdateQuery(){
	for(unsigned i=0; i<querys; i++)
		this->availability[i] = (float)rand()/RAND_MAX;
}

unsigned TLoadBalancer::getAvailable(){
	unsigned ti = 0;
	float min  = availability[0];
	srand(time(NULL));
	for(unsigned i=1; i<querys; i++){
		if(availability[i]<min){
			min = availability[i];
			ti = i;
		}
	}
	return ti;
}

int TLoadBalancer::getMaxFD(){
	int tmp = SockFD;
	for(unsigned i=0; i<m_clients.size(); i++){
		if(m_clients[i].first > tmp){
			tmp = m_clients[i].first;
		}
	}

	return tmp;
}

void TLoadBalancer::rmFD(int _fd){
	std::vector<std::pair<int, std::string> > tmp;
	for(unsigned i=0; i<m_clients.size(); i++){
		if(m_clients[i].first != _fd){
			tmp.push_back(m_clients[i]);
		}
	}
	m_clients = tmp;
}

void TLoadBalancer::closeClients(){
	std::vector<std::pair<int, std::string> > tmp;
	for(unsigned i=0; i<m_clients.size(); i++){
		close(m_clients[i].first);
		FD_CLR(m_clients[i].first, &master);
	}
	m_clients.clear();
}

void TLoadBalancer::onListening(){
	// do this with the protocol!!
	int buffer_size = 256;
    char buffer[buffer_size];

    FD_ZERO(&master);
    // add our first socket (server!!)
    FD_SET(SockFD, &master);

	int ConnectFD;
	std::cout << "Listening\n";

	fd_set copy;

	sockaddr_in NewSockAddr;
	socklen_t NewSockAddrSize = sizeof(NewSockAddr);

	int SockCount;
	int Sock;
	int Client;
	int maxFD;
	int activity;
	int check;
	std::string text;
	
	while(Connect){
		copy = master;
		maxFD = getMaxFD();
		activity = select(maxFD+1, &copy, NULL, NULL, NULL);

		if ((activity < 0) and (errno!=EINTR)){ 
			printf("Select error");
		}

		if(FD_ISSET(SockFD, &master)){
			Client = accept(SockFD, (sockaddr *)&NewSockAddr, &NewSockAddrSize);
			if(Client < 0){
				perror("Error accept failed");
			    exit(1);
			}
			std::cout << "fd: " << Client << "\tip: " << inet_ntoa(NewSockAddr.sin_addr) << "\n";
			/* text = "Welcome"+std::string(inet_ntoa(NewSockAddr.sin_addr));
			write(Client, text.c_str(), text.size()+1);*/

			FD_SET(Client, &master);
			m_clients.push_back(std::make_pair(Client, inet_ntoa(NewSockAddr.sin_addr)));			
		}

		for(unsigned i=0; i<m_clients.size(); i++){
			Client = m_clients[i].first;
			if(FD_ISSET(Client, &master)){
				check = read(Client, buffer, buffer_size);
				if(check < 0){
					// perror("Error Reading from Socket");
					FD_CLR(Client, &master);
					rmFD(Client);
					close(Client);
				}
				else{
					onUpdateQuery();
					// text = to_str(getAvailable());
					// write(Client, text.c_str(), text.size()+1);
					printf("[Client]: %s\t%u\n",buffer, getAvailable());
				}
			}
		}
	}

	closeClients();	
	onExit();
}

#endif