#include <iostream>
#include "src/load-balancer.h"

// g++ -std=c++11 -pthread balancer.cpp -o balancer.out
int main(int argc, char const *argv[]){
	int cport = 8888;		// port to clients
	int qport = 7777;		// port to query's server
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