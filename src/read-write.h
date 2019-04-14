#ifndef _READ_WRITE_H_
#define _READ_WRITE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>

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

#endif