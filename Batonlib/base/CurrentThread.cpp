//by Zhujunjie
#include "CurrentThread.h"

namespace CurrentThread
{
	pid_t gettid()
	{
		return static_cast<pid_t>(::syscall(SYS_gettid));
	}

	int cache() {
		t_cache_tid = gettid();
		snprintf(t_str_tid, sizeof t_cache_tid, "%5d ", t_cache_tid);
		return t_cache_tid;
	}
}