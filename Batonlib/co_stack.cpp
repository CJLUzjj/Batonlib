#include "co_stack.h"
#include <stdlib.h>

co_stack::co_stack(unsigned int stack_size)
{
    co_rut_ = nullptr;
    stack_size_ = stack_size;
    stack_sp_ = (char*)malloc(stack_size);
    stack_bp_ = stack_sp_ + stack_size_;
}

co_stack::~co_stack()
{
    free(stack_sp_);
}