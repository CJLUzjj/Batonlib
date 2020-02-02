//by Zhujunjie
#pragma once
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include <deque>
#include "Mutex.h"
#include "Condition.h"
#include "Thread.h"

class ThreadPool
{
public:
	typedef std::function<void ()> Task;
	explicit ThreadPool();
	~ThreadPool();
	void Set_Queue_Size(int size);
	void Set_Init_Thread_Pool_Callback(Task &c);
	void start(int num);
	void stop();
	void Thread_func();
	Task take();
	void put(Task task);
	bool isFull();
	std::string get_name();
	int Queue_size();
private:
	std::string name_;
	int Queue_max_size;
	Task ThreadInitCallback_;
	bool running_;
	mutable Mutex mutex_;
	Condition Full_cond_;
	Condition Empty_cond_;
	std::vector<std::unique_ptr<Thread>> threads_;
	std::deque<Task> Queue_;
};

