#ifndef _THREAD_U_H_
#define _THREAD_U_H_

#include <pthread.h>
#include <string>
#include <thread>
#include <mutex>
#include "socket-info.h"

class TThread{
public:
	std::thread m_tid;

	TThread();
	~TThread();

	void Create(void (*callback)(TSocket *), TSocket *dat);
	void Join();	
};

TThread::TThread(){

}

void TThread::Create(void (*callback)(TSocket *), TSocket *dat){
	this->m_tid = std::thread(callback, dat);
}

void TThread::Join(){
	m_tid.join();
}

#endif