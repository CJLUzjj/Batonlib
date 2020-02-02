#ifndef CO_STACK_H
#define CO_STACK_H

class coroutine;

class co_stack
{
public:
    coroutine* co_rut_;
    int stack_size_;
    char* stack_bp_;
    char* stack_sp_;
public:
    co_stack(unsigned int stack_size);
    ~co_stack();
};


#endif