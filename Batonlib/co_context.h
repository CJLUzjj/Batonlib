#ifndef CO_CONTEXT_H
#define CO_CONTEXT_H
#include <stdlib.h>
typedef void* (*ctx_func)(void* s1, void* s2);

//因为上下文需要对存储方式要求非常高，为了方便就不使用面向对象的方法，由coroutine封装
struct co_context
{
    void* regs[14];

    size_t s_size;//栈空间大小
    char* s_sp;//栈顶
};

int context_make(co_context* ctx, ctx_func func, const void* s1, const void* s2);
int context_init(co_context* ctx);

#endif