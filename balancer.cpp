#include <iostream>
#include "src/load-balancer.h"

int main(int argc, char const *argv[]){
	int port = 8888;
	TLoadBalancer *s = new TLoadBalancer();

	s->Connect(port);
	s->Listening();

	delete s;
	return 0;
}