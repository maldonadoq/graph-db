#ifndef _LOAD_BALANCER_H_
#define _LOAD_BALANCER_H_

#include "connection.h"
#include <queue>
#include <string>

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

void TLoadBalancer::onListening(){
	// do this with the protocol!!
	int buffer_size = 256;
    char buffer[buffer_size];

	int ClientFD;
	std::cout << "Listening\n";

	sockaddr_in ClientAddr;
	socklen_t ClientAddrSize = sizeof(ClientAddr);
	
	std::string text;
	pid_t childpid;

	int ret;
	unsigned idx;

	while(Connect){
		ClientFD = accept(SockFD, (struct sockaddr *)&ClientAddr, &ClientAddrSize);
		if(ClientFD < 0){
			perror("Error accept failed");
		    exit(1);
		}
		
		std::cout << "fd: " << ClientFD << "\tip: " << inet_ntoa(ClientAddr.sin_addr) << "\n";
		if((childpid = fork()) == 0){
			close(SockFD);

			//stop listening for new connections by the main process. 
			//the child will continue to listen. 
			//the main process now handles the connected client.
			while(true){
				onUpdateQuery();
				memset(buffer, 0, buffer_size);
				ret = recvfrom(ClientFD, buffer, buffer_size, 0, (struct sockaddr *) &ClientAddr, &ClientAddrSize);
				if(ret < 0){
					perror("Error Receiving Data from Client Socket");
					exit(1);
				}
				idx = getAvailable();
				printf("[%s]: %u\n", buffer, idx);

				text = "-> q[" + std::to_string(idx) + "]";
				ret = sendto(ClientFD, text.c_str(), text.size(), 0, (struct sockaddr *) &ClientAddr, ClientAddrSize);

				if(ret < 0){
					perror("Error Sending Data to Client Socket");
					exit(1);
				}
			}
		}
		close(ClientFD);
	}

	// onExit();
}

#endif