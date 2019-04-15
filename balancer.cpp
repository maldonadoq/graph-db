#include <iostream>
#include "src/load-balancer.h"

int main(int argc, char const *argv[]){
	int cport = 8888;
	int qport = 7777;
	TLoadBalancer *s = new TLoadBalancer();

	s->ClientPort(cport);
	s->QueryPort(qport);

	std::cout << "\n-------------------------------\n\n";

	std::thread client(TLoadBalancer::ListeningClients);
	std::thread query(TLoadBalancer::ListeningQuerys);

	client.join();
	query.join();
	//s->ListeningClients();

	delete s;
	return 0;
}