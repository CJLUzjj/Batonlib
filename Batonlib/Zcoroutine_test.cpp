#include "coroutine.h"
#include "scheduler.h"
#include "base/ThreadPool.h"
#include "base/Thread.h"
#include "tool.h"
#include <iostream>
#include <functional>
using namespace std;

void* func1(void* arg)
{
    int* start = (int*)arg;
    coroutine* co = coroutine::get_curr_co();
    for(int i = 0; i < 5; i++){
        cout<<"thread:"<<GetPid()<<" "<<CurrentThread::t_cache_tid<<" ";
        cout<<"coroutine "<<(*start)+i<<endl;
        co->yield();
    }
}

void thread_func()
{
    scheduler sch;
    int a = 0;
    int b = 100;
    coroutine co1(&sch, 1024, func1, &a);
    coroutine co2(&sch, 1024, func1, &b);
    while(!co1.end_ && !co2.end_){
        co1.resume();
        //cout<<"shift"<<endl;
        co2.resume();
    }
    sleep(1000);
}

int main()
{
    ThreadPool pool;
    pool.Set_Queue_Size(5);
    pool.start(3);
    function<void()> task;
    task = thread_func;
    for(int i = 0; i < 3; i++){
        pool.put(task);
    }
    sleep(1000);
    return 0;
}