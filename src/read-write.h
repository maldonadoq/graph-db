#ifndef _READ_WRITE_H_
#define _READ_WRITE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <chrono>

void thread_write(int _sockFD){
	std::string text;

	while(true){
		getline(std::cin, text);
		send(_sockFD, text.c_str(), text.size(), 0);
	}
}

void thread_read(int _sockFD){
	unsigned buffer_size = 256;
	char buffer[buffer_size];

	while(true){
		memset(&buffer, 0, buffer_size);
		if(recv(_sockFD, buffer, buffer_size, 0) > 0){			
			std::cout << buffer << "\n";
		}
	}
}

// query
void thread_qwrite(int _sockFD, int _sleep){
	std::string text;

	while(true){
		std::this_thread::sleep_for(std::chrono::milliseconds(_sleep));
		text = std::to_string(rand()%10);
		send(_sockFD, text.c_str(), text.size(), 0);
	}
}

void thread_qread(int _sockFD, int _sockDB){
	unsigned buffer_size = 256;
	char buffer[buffer_size];

	while(true){
		memset(&buffer, 0, buffer_size);
		if(recv(_sockFD, buffer, buffer_size, 0) > 0){			
			std::cout << buffer << "\n";
			send(_sockDB, buffer, buffer_size, 0);
		}
	}
}

#endif