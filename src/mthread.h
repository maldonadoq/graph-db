#ifndef _MTHREAD_H_
#define _MTHREAD_H_

#include <pthread.h>
#include <string>
#include <thread>
#include <mutex>
#include "data.h"

class TThread{
private:
	static std::mutex m_mutex;
public:
	std::thread m_tid;

	TThread();
	~TThread();

	void Create(void (*callback)(TData *), TData *dat);
	void Join();

	static void LockMutex(std::string);
	static void UnlockMutex(std::string);
};

std::mutex TThread::m_mutex;

TThread::TThread(){

}

void TThread::Create(void (*callback)(TData *), TData *dat){
	this->m_tid = std::thread(callback, dat);
}

void TThread::Join(){
	m_tid.join();
}

void TThread::LockMutex(std::string identifier){
	TThread::m_mutex.lock();
}

void TThread::UnlockMutex(std::string identifier){
	TThread::m_mutex.unlock();
}

#endif