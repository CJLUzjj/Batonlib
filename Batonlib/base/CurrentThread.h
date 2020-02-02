//by Zhujunjie
#pragma once
#include <syscall.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <unistd.h>
namespace CurrentThread
{
	extern __thread int t_cache_tid;
	extern __thread char t_str_tid[32];
	extern __thread int t_tid_str_size;
	extern __thread const char* t_thread_name;

	//pid_t gettid()
	//{
	//	return static_cast<pid_t>(::syscall(SYS_gettid));
	//}
	//
	//int cache() {
	//	t_cache_tid = gettid();
	//	snprintf(t_str_tid, sizeof t_cache_tid, "%5d ", t_cache_tid);
	//	return t_cache_tid;
	//}
	pid_t gettid();
	int cache();
	inline int tid() {
		return t_cache_tid;
	}

	inline char* str_tid()
	{
		return t_str_tid;
	}

	inline int tid_str_size()
	{
		return t_tid_str_size;
	}

	inline const char* thread_name()
	{
		return t_thread_name;
	}
}