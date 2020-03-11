#include "coroutine_stackless.h"

coroutine_stackless::coroutine_stackless(scheduler* sch, stackless_co_func func, stackless_co* arg)
    :counter_(0),
    sch_(sch),
    func_(func),
    arg_(arg)
{
    arg_->co = this;
}

coroutine_stackless::~coroutine_stackless()
{
}

void coroutine_stackless::resume()
{
    func_(arg_);
}