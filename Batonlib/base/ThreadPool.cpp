//by Zhujunjie
#include "ThreadPool.h"


typedef std::function<void()> Task;
ThreadPool::ThreadPool()
   :name_("Thread_Pool"),
	mutex_(),
	Full_cond_(mutex_),
	Empty_cond_(mutex_),
	Queue_max_size(100),
	running_(false)
{
}


ThreadPool::~ThreadPool()
{
}

void ThreadPool::Set_Queue_Size(int size)
{
	Queue_max_size = size;
}

void ThreadPool::Set_Init_Thread_Pool_Callback(Task &c)
{
	ThreadInitCallback_ = c;
}

void ThreadPool::start(int num)
{
	assert(threads_.empty());
	threads_.reserve(num);
	running_ = true;
	for (int i = 0; i < num; ++i) {
		char id[32];
		snprintf(id, sizeof id, "_%d", i + 1);
		//bind绑定成员函数时要加作用域
		threads_.emplace_back(new Thread(std::bind(&ThreadPool::Thread_func, this), name_ + id));
		threads_[i]->start();
	}
}

Task ThreadPool::take()
{
	MutexLockGuard lock(mutex_);
	while (Queue_.empty() && running_ ) {
		Empty_cond_.Wait();
	}
	Task task;
	if (!Queue_.empty()) {		//可能stop调用了notify所以有可能依然为空
		task = Queue_.front();
		Queue_.pop_front();
		Full_cond_.Notify();
	}
	return task;
}

void ThreadPool::Thread_func()
{
	while (running_)
	{
		Task task(take());
		//一定要加条件判断！！！否则stop唤醒所有empty_wait线程后会传来一个
		//空的task，就会抛出一个bad_function_call异常，血的教训！！！
		if (task) {
			task();
		}
	}
}

void ThreadPool::stop()
{
	{
		MutexLockGuard lock(mutex_);
		running_ = false;
		Empty_cond_.NotifyAll();
	}
	for (auto&thr : threads_) {
		thr->join();
	}
}

bool ThreadPool::isFull()
{
	return Queue_.size() >= Queue_max_size;		//不能加锁，否则会死锁
}

void ThreadPool::put(Task task)
{
	MutexLockGuard lock(mutex_);
	while (isFull())
	{
		Full_cond_.Wait();
	}
	Queue_.push_back(task);
	Empty_cond_.Notify();
}

std::string ThreadPool::get_name()
{
	return name_;
}

int ThreadPool::Queue_size()
{
	MutexLockGuard lock(mutex_);
	return Queue_.size();
}