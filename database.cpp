#include <iostream>
#include "src/database.h"

// g++ -std=c++11 -pthread database.cpp -o database.out -lsqlite3
int main(int argc, char const *argv[]){
		
	std::string ip = "192.168.1.41";
	int port = 6666;
	std::string name = "src/first-db.db";

	TDatabase *db = new TDatabase(name);
	db->Connect(ip, port);
	db->Talking();

	delete db;
	return 0;
}