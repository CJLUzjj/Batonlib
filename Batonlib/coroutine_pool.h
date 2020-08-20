#include "coroutine.h"
#include "channel.h"
#include "scheduler.h"
#include <functional>

struct pool_arg {
    int index;
    coroutine_pool* pool;
};

struct task {
    function<void(int)> func;
};

class coroutine_pool
{
private:
    int coroutine_cnt_;
    channel<task> chan_;
    scheduler* sch_;
public:
    coroutine_pool(int work_cnt, int stack_size);
    ~coroutine_pool();
    static void* coroutine_start(void* arg);
    void work_start(int index);
    void run(function<void(int)> func);
};