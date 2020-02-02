//by Zhujunjie
#include "Condition.h"

Condition::Condition(Mutex& mutex) :mutex_(mutex)
{
	pthread_cond_init(&cond_, NULL);
}

Condition::~Condition()
{
	pthread_cond_destroy(&cond_);
}

void Condition::Wait()
{
	assert(mutex_.isLock());
	pthread_cond_wait(&cond_, mutex_.GetMutex());
	mutex_.restoreMutexStatue();
}

void Condition::Notify()
{
	pthread_cond_signal(&cond_);
}

void Condition::NotifyAll()
{
	pthread_cond_broadcast(&cond_);
}

bool Condition::WaitSomeSecond(int second)
{
	struct timespec abstime;
	clock_gettime(CLOCK_REALTIME, &abstime);
	abstime.tv_sec += static_cast<time_t>(second);
	return ETIMEDOUT == pthread_cond_timedwait(&cond_, mutex_.GetMutex(), &abstime);
}