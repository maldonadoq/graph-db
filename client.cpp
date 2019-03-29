#include <iostream>
#include "src/client.h"

int main(int argc, char const *argv[]){
		
	// std::string ip = "127.0.0.1";
	std::string ip = "192.168.1.39";
	int port = 5555;

	TClient *cl = new TClient();
	cl->onConnect(ip, port);
	cl->onTalking();

	delete cl;
	return 0;
}