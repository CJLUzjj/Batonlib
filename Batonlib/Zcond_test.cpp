#include "co_condition.h"
#include "scheduler.h"
#include "co_sys_call.h"
#include <queue>
#include <iostream>
using namespace std;


struct task{
    co_condition* cond;
    queue<int> task_queue;
};

void* Consumer(void* arg)
{
    task* t = (task*)arg;

    while(1){
        if(t->task_queue.empty()){
            t->cond->co_cond_wait();
            continue;
        }
        int val = t->task_queue.front();
        t->task_queue.pop();
        cout<<"consumer get task:"<<val<<endl;
    }
    return NULL;
}

void* Producer(void* arg)
{
    task* t = (task*)arg;
    int val = 0;
    while(1){
        t->task_queue.push(val);
        cout<<"producer put task:"<<val++<<endl;
        t->cond->co_cond_signal();
        co_register(-1, 0, 1000);
    }
}

int main()
{
    scheduler* sch = new scheduler();
    task* t = new task;
    t->cond = new co_condition();

    coroutine* consumer = new coroutine(sch, 1024*1024, [](void* arg) ->void*{
        task* t = (task*)arg;

        while(1){
            if(t->task_queue.empty()){
                t->cond->co_cond_wait();
                continue;
            }
            int val = t->task_queue.front();
            t->task_queue.pop();
            cout<<"consumer get task:"<<val<<endl;
        }
        return NULL;
    }, t);
    consumer->resume();

    coroutine* producer = new coroutine(sch, 1024*1024, Producer, t);
    producer->resume();

    sch->event_loop(NULL, NULL);

    return 0;
}