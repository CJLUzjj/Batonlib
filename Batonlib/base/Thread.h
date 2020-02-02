//by Zhujunjie
#pragma once
#include <sys/syscall.h>
#include <sys/prctl.h>
#include <functional>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <string>
#include "CountDownLatch.h"
#include "CurrentThread.h"
class Thread
{
public:
	typedef std::function<void()> ThreadFunction;
	explicit Thread(const ThreadFunction func, std::string name = std::string());
	~Thread();
	void start();
	int join();
	bool is_start() const { return start_; }
	const std::string& getThreadName() const {return Thread_name_;}
	pid_t get_tid() const { return tid_; }
	static void* startfunc(void *);
private:
	std::string Thread_name_;
	ThreadFunction func_;
	pthread_t pthread_id_;
	CountDownLatch L_;
	bool start_;
	bool join_;
	pid_t tid_;
};

