//by Zhujunjie
#include "AsyncLogging.h"



AsyncLogging::AsyncLogging(const std::string basename, int flushInterval):
	basename_(basename),
	flush_time_(flushInterval),
	running_(false),
	mutex_(),
	cond_(mutex_),
	L_(1),
	thread_(std::bind(&AsyncLogging::thread_func_, this), "LogThread"),
	Current_Buffer_(new Buffer),
	Next_Buffer_(new Buffer),
	Buffers_()
{
	assert(basename_.size() > 1);
	Current_Buffer_->bzero();
	Next_Buffer_->bzero();
	Buffers_.reserve(16);
}


AsyncLogging::~AsyncLogging()
{
	if(running_)
		stop();
}

void AsyncLogging::stop()
{
	running_ = false;
	cond_.Notify();
	thread_.join();
}

void AsyncLogging::start()
{
	running_ = true;
	thread_.start();
	L_.wait();
}

void AsyncLogging::append(const char* line, int len)
{
	MutexLockGuard lock(mutex_);
	if (Current_Buffer_->avail() > len) {
		Current_Buffer_->append(line, len);
	}
	else {
		Buffers_.push_back(Current_Buffer_);
		Current_Buffer_.reset();
		if (Next_Buffer_) {
			Current_Buffer_ = std::move(Next_Buffer_);
		}
		else {
			Current_Buffer_.reset(new Buffer);
		}
		Current_Buffer_->append(line, len);
		cond_.Notify();
	}
}

void AsyncLogging::thread_func_()
{
	assert(running_ == true);
	L_.Countdown();
	LogFile output(basename_);
	BufferPtr newBuffer1(new Buffer);
	BufferPtr newBuffer2(new Buffer);
	BufferPtrs Buffer_To_Write;
	newBuffer1->bzero();
	newBuffer2->bzero();
	Buffer_To_Write.reserve(16);
	while (running_)
	{
		assert(newBuffer1 && newBuffer1->length() == 0);
		assert(newBuffer2 && newBuffer2->length() == 0);
		{
			MutexLockGuard lock(mutex_);
			if (Buffers_.empty()) {	//非常规用法
				cond_.WaitSomeSecond(flush_time_);
			}
			Buffers_.push_back(Current_Buffer_);
			Current_Buffer_.reset();
			Current_Buffer_ = std::move(newBuffer1);
			Buffer_To_Write.swap(Buffers_);
			if (!Next_Buffer_) {
				Next_Buffer_ = std::move(newBuffer2);
			}
		}

		assert(!Buffer_To_Write.empty());
		if (Buffer_To_Write.size() > 25) {	//写太多了，非正常情况
			Buffer_To_Write.erase(Buffer_To_Write.begin() + 2, Buffer_To_Write.end());
		}
		for (size_t i = 0; i < Buffer_To_Write.size(); i++) {
			output.append(Buffer_To_Write[i]->data(), Buffer_To_Write[i]->length());
		}
		if (Buffer_To_Write.size() > 2) {
			Buffer_To_Write.resize(2);
		}
		if (!newBuffer1) {
			assert(!Buffer_To_Write.empty());
			newBuffer1 = std::move(Buffer_To_Write.back());
			Buffer_To_Write.pop_back();
			newBuffer1->reset();
		}
		if (!newBuffer2) {
			assert(!Buffer_To_Write.empty());
			newBuffer2 = std::move(Buffer_To_Write.back());
			Buffer_To_Write.pop_back();
			newBuffer2->reset();
		}

		Buffer_To_Write.clear();
		output.flush();
	}
	output.flush();
}