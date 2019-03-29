#include <iostream>
#include "src/load-balancer.h"

int main(int argc, char const *argv[]){
	
	int port = 4444;
	int n = 5;
	int q = 3;

	TLoadBalancer *lb = new TLoadBalancer(q);
	lb->onConnect(port);
	lb->onListening();

	delete lb;
	return 0;
}