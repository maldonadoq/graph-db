#include <iostream>
#include <thread>
#include "src/query.h"

// g++ -std=c++11 -pthread query.cpp -o query.out
int main(int argc, char const *argv[]){
		
	std::string ip = "192.168.1.41";
	int bport = 7777;
	int dport = 6666;

	TQuery *q = new TQuery();
	q->ConnectToBalancer(ip, bport);
	q->CreateServerQuery(dport);

	std::thread talk(TQuery::TalkingToBalancer);	
	std::thread listen(TQuery::ListeningDb);

	talk.join();
	listen.join();
	// q->Talking();

	delete q;
	return 0;
}