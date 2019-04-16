#include <iostream>
#include "src/client.h"

// g++ -std=c++11 -pthread client.cpp -o client.out
int main(int argc, char const *argv[]){
		
	std::string ip = "192.168.1.41";
	int port = 8888;

	TClient *cl = new TClient();
	cl->Connect(ip, port);
	cl->Talking();

	delete cl;
	return 0;
}