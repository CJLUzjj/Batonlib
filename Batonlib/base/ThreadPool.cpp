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
		//bind�󶨳�Ա����ʱҪ��������
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
	if (!Queue_.empty()) {		//����stop������notify�����п�����ȻΪ��
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
		//һ��Ҫ�������жϣ���������stop��������empty_wait�̺߳�ᴫ��һ��
		//�յ�task���ͻ��׳�һ��bad_function_call�쳣��Ѫ�Ľ�ѵ������
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
	return Queue_.size() >= Queue_max_size;		//���ܼ��������������
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