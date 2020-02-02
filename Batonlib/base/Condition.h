//by Zhujunjie
#pragma once
#include <pthread.h>
#include <errno.h>
#include "Mutex.h"

class Condition
{
public:
	Condition(Mutex& mutex);
	~Condition();
	void Wait();
	void Notify();
	void NotifyAll();
	bool WaitSomeSecond(int second);

private:
	Mutex& mutex_;		//ÒýÓÃ
	pthread_cond_t cond_;
};

//Condition::Condition(Mutex& mutex):mutex_(mutex)
//{
//	pthread_cond_init(&cond_, NULL);
//}
//
//Condition::~Condition()
//{
//	pthread_cond_destroy(&cond_);
//}
//
//void Condition::Wait()
//{
//	assert(mutex_.isLock());
//	pthread_cond_wait(&cond_, mutex_.GetMutex());
//	mutex_.restoreMutexStatue(); 
//}
//
//void Condition::Notify()
//{
//	pthread_cond_signal(&cond_);
//}
//
//void Condition::NotifyAll()
//{
//	pthread_cond_broadcast(&cond_);
//}
//
//bool Condition::WaitSomeSecond(int second)
//{
//	struct timespec abstime;
//	clock_gettime(CLOCK_REALTIME, &abstime);
//	abstime.tv_sec += static_cast<time_t>(second);
//	return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex_.GetMutex(), &abstime);
//}