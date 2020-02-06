#ifndef CO_CONDITION_H
#define CO_CONDITION_H

#include "scheduler.h"
#include "coroutine.h"
#include "co_epoll.h"
#include <deque>

using namespace std;

class co_condition
{
private:
    deque<co_epoll_item*> co_deque_;
    scheduler* sch_;
public:
    co_condition();
    co_condition(scheduler*);
    ~co_condition();

    int co_cond_wait();

    int co_cond_signal();

    int co_cond_broadcast();
};


#endif