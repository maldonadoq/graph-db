#include <iostream>
#include "src/client.h"

int main(int argc, char const *argv[]){
		
	std::string ip = "192.168.1.41";
	int port = 8888;

	TClient *cl = new TClient();
	cl->Connect(ip, port);
	cl->Talking();

	delete cl;
	return 0;
}