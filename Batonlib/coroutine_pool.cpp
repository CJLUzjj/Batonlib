#include "coroutine_pool.h"
#include "scheduler.h"

coroutine_pool::coroutine_pool(int work_cnt, int stack_size)
    :coroutine_cnt_(work_cnt),
    sch_(scheduler::get_curr_thr_sch()),
    chan_()
{
    for(int i = 0; i < work_cnt; i++) {
        pool_arg* arg = new pool_arg;
        arg->index = i;
        arg->pool = this;
        coroutine* co = new coroutine(sch_, stack_size, coroutine_start, arg);
        co->resume();
    }
}

coroutine_pool::~coroutine_pool()
{
}

void* coroutine_pool::coroutine_start(void* arg) {
    pool_arg* parg = (pool_arg*)arg;
    coroutine_pool* pool = parg->pool;
    pool->work_start(parg->index);
}

void coroutine_pool::work_start(int index) {
    while(1) {
        task t;
        chan_.get(t);
        t.func(index);
        delete &t;
    }
}

void coroutine_pool::run(function<void(int)> func) {
    task* t = new task;
    t->func = func;
    chan_.put(*t);
}