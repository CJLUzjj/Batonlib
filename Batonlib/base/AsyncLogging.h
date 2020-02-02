//by Zhujunjie
#pragma once

#include "Mutex.h"
#include "CountDownLatch.h"
#include "Thread.h"
#include "LogStream.h"
#include "LogFile.h"
#include <functional>
#include <unistd.h>
#include <memory>
#include <vector>
#include <string>

class AsyncLogging
{
public:
	AsyncLogging(const std::string basename, int flushInterval = 2);
	~AsyncLogging();
	void start();
	void stop();
	void append(const char* line, int len);
private:
	void thread_func_();
	typedef Log_Buffer Buffer;
	typedef std::vector<std::shared_ptr<Buffer>> BufferPtrs;
	typedef std::shared_ptr<Buffer> BufferPtr;
	int flush_time_;
	bool running_;
	const std::string basename_;
	Thread thread_;
	Mutex mutex_;
	Condition cond_;
	CountDownLatch L_;
	BufferPtr Current_Buffer_;
	BufferPtr Next_Buffer_;
	BufferPtrs Buffers_;
};

