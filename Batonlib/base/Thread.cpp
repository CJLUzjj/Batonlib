//by Zhujunjie
#include "Thread.h"

namespace CurrentThread
{
	__thread int t_cache_tid;
	__thread char t_str_tid[32];
	__thread int t_tid_size = 6;
	__thread const char* t_thread_name = "default";
}
struct Thread_Data
{
	typedef std::function<void()> ThreadFunction;
	ThreadFunction func_;
	std::string Thread_name_;
	pid_t *ptr_tid_;
	CountDownLatch *ptr_L_;

	Thread_Data(ThreadFunction& func, std::string& Thread_name, pid_t *ptr_tid, CountDownLatch *ptr_L)
		:func_(func),
		Thread_name_(Thread_name),
		ptr_tid_(ptr_tid),
		ptr_L_(ptr_L)
	{	}

	void run_in_thread()
	{
		*ptr_tid_ = CurrentThread::cache();
		ptr_L_->Countdown();
		CurrentThread::t_thread_name = Thread_name_.empty() ? "Thread" : Thread_name_.c_str();
		prctl(PR_SET_NAME, CurrentThread::t_thread_name);
		func_();
		CurrentThread::t_thread_name = "Finish";
	}
};

Thread::Thread(const ThreadFunction func, std::string name)
	:Thread_name_(name),
	 func_(func),
	 pthread_id_(0),
	 L_(1),
	 start_(false),
	 join_(false),
	 tid_(0)
{
	if (name.empty()) {
		char buf[32];
		snprintf(buf, sizeof buf, "Thread");
		Thread_name_ = buf;
	}
}


Thread::~Thread()
{
	if (start_ && !join_) {
		pthread_detach(pthread_id_);
	}
}

void* Thread::startfunc(void * obj)
{
	Thread_Data* data = static_cast<Thread_Data*>(obj);
	data->run_in_thread();
	delete data;
	return NULL;
}

void Thread::start()
{
	assert(!start_);
	start_ = true;
	Thread_Data *data = new Thread_Data(func_, Thread_name_, &tid_, &L_);
	if (pthread_create(&pthread_id_, NULL, startfunc, data)) {
		start_ = false;
		delete data;
	}
	else {
		L_.wait();
		assert(tid_ > 0);
	}
}

int Thread::join()
{
	assert(start_);
	assert(!join_);
	join_ = true;
	return pthread_join(pthread_id_, NULL);
}