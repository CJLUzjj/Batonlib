#include "co_stack.h"
#include <stdlib.h>
#include <sys/mman.h>
#include <assert.h>

const int pageSize = 4096;

co_stack::co_stack(unsigned int stack_size, bool protect_mode)
    :co_rut_(nullptr),
    stack_size_(stack_size)
{

    if(!protect_mode) {
        stack_sp_ = (char*)malloc(stack_size);
        stack_bp_ = stack_sp_ + stack_size_;
        return;
    }

    stack_sp_ = (char*)mmap(NULL, 2 * pageSize + stack_size, PROT_READ | PROT_WRITE, 
            MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

    if(stack_sp_ == (void*)-1 || stack_sp_ == 0) {
        assert(false);
    }

    mprotect(stack_sp_, pageSize, PROT_NONE);
    mprotect(stack_sp_ + pageSize + stack_size, pageSize, PROT_NONE);
    stack_bp_ = stack_sp_ + pageSize + stack_size;
}

co_stack::~co_stack()
{
    free(stack_sp_);
}