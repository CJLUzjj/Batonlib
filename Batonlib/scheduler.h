#ifndef __SCHEDULER_H__
#define __SCHEDULER_H__
#include <stack>
#include <map>
#include <queue>
#include <pthread.h>
#include "coroutine.h"
#include "time_wheel.h"

using namespace std;

class co_epoll;

class scheduler
{
public:
    typedef int (*eventloopfunc)(void*);

    stack<coroutine*> co_call_stack_;//协程调用栈
    int stack_max_size_;//栈的最大值

    co_epoll* sch_epoll_;

    size_t co_size_;

    time_wheel* tw_;

    queue<coroutine*> destroy_co_;
public:
    scheduler();
    ~scheduler();

    static scheduler* get_curr_thr_sch();

    static coroutine* get_curr_thr_co();

    int register_event(int fd, __int32_t events, int timeout);

    void event_loop(eventloopfunc func, void* arg);

    void clear_co();

    //static和全局变量在.h文件中只是作为声明，定义需要在.cpp中定义
    static map<pid_t, scheduler*> g_thr_to_sch;
};

#endif

