//by Zhujunjie
#include "CountDownLatch.h"

CountDownLatch::CountDownLatch(int count) : count_(count), mutex_(), cond_(mutex_)
{
}

CountDownLatch::~CountDownLatch()
{
}

void CountDownLatch::wait()
{
	MutexLockGuard lock(mutex_);
	while (count_ > 0)
	{
		cond_.Wait();
	}
}

void CountDownLatch::Countdown()
{
	MutexLockGuard lock(mutex_);
	--count_;
	if (count_ == 0) {
		cond_.NotifyAll();
	}
}

int CountDownLatch::getCount() const
{
	MutexLockGuard lock(mutex_);
	return count_;
}