#ifndef __COROUTINE_H__
#define __COROUTINE_H__
#include "co_context.h"
#include <functional>

using namespace std;

typedef function<void*(void*)> coroutine_func;

class co_stack;
class scheduler;
class connection;

class coroutine
{
public:
    scheduler* sch_;

    coroutine_func func_;
    void* arg_;

    co_context ctx_;

    bool start_;
    bool end_;
    bool ismain_;
    
    co_stack* cst_;

    char* stack_sp_;
    unsigned int save_size_;
    char* save_buf_;//协程挂起时，栈内容存储的位置

    connection* conn_;

public:
    coroutine(scheduler* sch);

    coroutine(scheduler* sch, int stack_size, coroutine_func func, void* arg);
    ~coroutine();

    void resume();
    static void yield();

    static coroutine* get_curr_co();

    void swap(coroutine*);

    void setconn(connection*);
};

static int co_func_wrapper(coroutine* co, void*);

#endif