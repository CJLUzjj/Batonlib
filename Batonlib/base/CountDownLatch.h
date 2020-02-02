//by Zhujunjie
#pragma once
#include "Mutex.h"
#include "Condition.h"
class CountDownLatch
{
public:
	CountDownLatch(int count);
	~CountDownLatch();
	void wait();
	void Countdown();
	int getCount() const;
private:
	mutable Mutex mutex_;
	Condition cond_;
	int count_;
};

//CountDownLatch::CountDownLatch(int count): count_(count),mutex_(),cond_(mutex_)
//{
//}
//
//CountDownLatch::~CountDownLatch()
//{
//}
//
//void CountDownLatch::wait()
//{
//	MutexLockGuard lock(mutex_);
//	while (count_ > 0)
//	{
//		cond_.Wait();
//	}
//}
//
//void CountDownLatch::Countdown()
//{
//	MutexLockGuard lock(mutex_);
//	--count_;
//	if (count_ == 0) {
//		cond_.NotifyAll();
//	}
//}
//
//int CountDownLatch::getCount() const
//{
//	MutexLockGuard lock(mutex_);
//	return count_;
//}