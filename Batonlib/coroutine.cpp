#include "coroutine.h"
#include "co_stack.h"
#include "scheduler.h"
#include "net/connection.h"
#include <iostream>

extern "C"
{
    extern void context_swap(co_context*, co_context*) asm("context_swap");
}

void* co_func_wrapper(void* vco, void*)
{
    coroutine* co = (coroutine*)vco;
    if(co->func_){
        co->func_(co->arg_);
    }

    co->end_ = 1;
    co->sch_->destroy_co_.push(co);

    co->yield();
    return 0;
}

coroutine::coroutine(scheduler* sch)
    :sch_(sch),
    func_(NULL),
    arg_(NULL),
    start_(0),
    end_(0),
    ismain_(1),
    cst_(NULL),
    save_size_(0),
    save_buf_(NULL)
{

}

coroutine::coroutine(scheduler* sch, int stack_size, coroutine_func func, void* arg)
    :sch_(sch),
    func_(func),
    arg_(arg),
    start_(0),
    end_(0),
    ismain_(0),
    cst_(new co_stack(stack_size, false)),
    save_size_(0),
    save_buf_(NULL),
    conn_(NULL)
{
    ctx_.s_size = stack_size;
    ctx_.s_sp = cst_->stack_sp_;
}


coroutine::~coroutine()
{
    delete cst_;
    if(conn_)
        delete conn_;
}

//这里有一个疑问，就是如果resume的是另一个线程的协程，会产生什么情况？
void coroutine::resume()
{
    scheduler* sch = sch_;

    coroutine* co1 = sch->co_call_stack_.top();//当前协程

    if(!start_){
        context_make(&ctx_, co_func_wrapper, this, 0);
        start_ = 1;
    }

    sch->co_call_stack_.push(this);
    //cout<<"push "<<(long long)this<<endl;

    co1->swap(this);
}

void coroutine::yield()
{
    scheduler* sch = scheduler::get_curr_thr_sch();
    coroutine* curr = sch->co_call_stack_.top();
    sch->co_call_stack_.pop();//疑似有内存泄漏问题，用户需持有被yield协程的指针
    //cout<<"pop "<<(long long)curr<<endl;
    coroutine* last = sch->co_call_stack_.top();
    curr->swap(last);
}

//参数是要切换的协程，被切换的协程为this
void coroutine::swap(coroutine* co1)
{
    //如果是每个协程一个栈的话，就不需要保存栈的内容
    context_swap(&this->ctx_, &co1->ctx_);
}

void coroutine::setconn(connection* conn)
{
    conn_ = conn;
}

coroutine* coroutine::get_curr_co()
{
    scheduler* sch =  scheduler::get_curr_thr_sch();
    return sch->co_call_stack_.top();
}