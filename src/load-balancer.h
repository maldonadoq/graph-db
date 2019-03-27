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
	unsigned onAvailable();
};

void TLoadBalancer::onUpdateQuery(){
	for(unsigned i=0; i<querys; i++)
		this->availability[i] = (float)rand()/RAND_MAX;
}

unsigned TLoadBalancer::onAvailable(){
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

    int n;
    unsigned q;
    std::string recvd = "Received";

	int ConnectFD;
	std::cout << "Listening\n";
	
	while(Connect){
		onUpdateQuery();

		ConnectFD = accept(SockFD, NULL, NULL);
		std::cout << ConnectFD << " ";
		if(0 > ConnectFD){
	    	perror("Error accept failed");
	    	exit(1);
		}
 
 		memset(&buffer, 0, buffer_size);
		n = read(ConnectFD, buffer, buffer_size-1);

		if (n < 0){
			perror("Error Reading from Socket");		
		}

		n = write(ConnectFD,recvd.c_str(),recvd.size());
     	if (n < 0){
     		perror("Error Writing to socket");
     	}

		q = onAvailable();
		printf("[Client]: %s\t%u\n",buffer, q);		
	}
	close(ConnectFD);
	onExit();
}

#endif