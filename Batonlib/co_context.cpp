#include "co_context.h"
#include <string.h>

//reg[0]: r15   低地址
//reg[1]: r14
//reg[2]: r13
//reg[3]: r12
//reg[4]: r9
//reg[5]: r8
//reg[6]: rbp   栈底
//reg[7]: rdi   第一个参数地址
//reg[8]: rsi   第二个参数地址
//reg[9]: ret   返回函数
//reg[10]: rdx
//reg[11]: rcx
//reg[12]: rbx
//reg[13]: rsp  栈顶

enum{
    kRDI = 7,
    kRSI = 8,
    kRET = 9,
    kRSP = 13,
};

extern "C"
{
    extern void context_swap(co_context*, co_context*) asm("context_swap");
}

//初始化协程，这是在切换协程前要做的，构建上下文
int context_make(co_context* ctx, ctx_func func, const void* s1, const void* s2)
{
    char* sp = ctx->s_sp + ctx->s_size - sizeof(void*);//栈底
    sp = (char*) ((unsigned long)sp & -16LL);//栈底与1111111....101111做与运算，应该是用于内存对齐
    
    memset(ctx->regs, 0, sizeof(ctx->regs));

    void** retadress = (void**)sp;

    *retadress = (void*)func;
    
    ctx->regs[kRSP] = sp;
    ctx->regs[kRET] = (char*)func;//返回地址
    ctx->regs[kRDI] = (char*)s1;
    ctx->regs[kRSI] = (char*)s2;
    return 0;
}

//初始化主协程，因为主协程必定是先被切换，所以无需给他构造上下文
int context_init(co_context* ctx)
{
    memset(ctx, 0, sizeof(co_context));
    return 0;
}