#ifndef COROUTINE_STACKLESS_H
#define COROUTINE_STACKLESS_H
#include <functional>
#include "scheduler.h"
using namespace std;

#define CO_REENTER(co) \
    switch(int &flag = co->get_counter()) \
        case -1: if(flag) \
        { \
            flag = -1; \
            goto bail_out_of_coroutine; \
        bail_out_of_coroutine: \
            break; \
        }else case 0:



#define CO_YEILD(n) \
    for(flag = (n); ; ) \
        if(flag == 0) \
        { \
            case (n): ;\
            break; \
        }else { \
            goto bail_out_of_coroutine; \
        }

#define RESUME(co) co->resume();

#define YEILD CO_YEILD(__COUNTER__ + 1)

class coroutine_stackless;

struct stackless_co{
    void* user_data1;
    void* user_data2;
    coroutine_stackless* co;
};

typedef function<void(stackless_co*)> stackless_co_func;

class coroutine_stackless
{
public:
    coroutine_stackless(scheduler* sch, stackless_co_func func, stackless_co* arg);
    ~coroutine_stackless();
    int add(){counter_++; return counter_;}
    int& get_counter(){return counter_;}
    void resume();
    int counter_;
private:
    scheduler* sch_;
    stackless_co_func func_;
    stackless_co* arg_;
};

#endif