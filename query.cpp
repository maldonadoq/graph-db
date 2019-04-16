#include <iostream>
#include "src/query.h"

int main(int argc, char const *argv[]){
		
	std::string ip = "192.168.1.41";
	int port = 7777;

	TQuery *q = new TQuery();
	q->Connect(ip, port);
	q->Talking();

	delete q;

	return 0;
}