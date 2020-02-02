//by Zhujunjie
#pragma once
#include <pthread.h>
#include <assert.h>

class Mutex
{
public:
	friend class Condition;
	Mutex();
	~Mutex();
	void Lock();
	void UnLock();
	bool isLock() const;
	pthread_mutex_t* GetMutex();
	void restoreMutexStatue();

private:
	bool isLock_;
	pthread_mutex_t mutex_;
};
//Mutex::Mutex():isLock_(false)
//{
//	pthread_mutex_init(&mutex_, NULL);
//}
//
//Mutex::~Mutex()
//{
//	assert(!isLock_);
//	pthread_mutex_destroy(&mutex_);
//}
//
//void Mutex::Lock()
//{
//	pthread_mutex_lock(&mutex_);
//	isLock_ = true;
//}
//
//void Mutex::UnLock()
//{
//	pthread_mutex_unlock(&mutex_);
//	isLock_ = false;
//}
//
//bool Mutex::isLock() const
//{
//	return isLock_;
//}
//
//pthread_mutex_t* Mutex::GetMutex()
//{
//	return &mutex_;
//}
//
//void Mutex::restoreMutexStatue()
//{
//	isLock_ = true;
//}
//
////------------------------------------------------------------------------------------
//
class MutexLockGuard
{
public:
	MutexLockGuard(Mutex& mutex);
	~MutexLockGuard();

private:
	Mutex& mutex_;	//ÒýÓÃ
};
//
//MutexLockGuard::MutexLockGuard(Mutex& mutex):mutex_(mutex)
//{
//	mutex_.Lock();
//}
//
//MutexLockGuard::~MutexLockGuard()
//{
//	mutex_.UnLock();
//}