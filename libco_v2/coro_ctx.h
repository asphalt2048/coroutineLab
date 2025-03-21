#pragma once

#include <stdlib.h>

namespace coro {

using func_t = void (*)(void *);

struct context {
    void *rdi=0;
    void *rsi=0;
    void *rdx=0;
    void *rcx=0;
    void *r8=0;
    void *r9=0;
    void *ret=0;//ret address
    void *rbp=0;
    void *rbx=0;
    void *r12=0;
    void *r13=0;
    void *r14=0;
    void *r15=0;
    void *rsp=0;


    size_t ss_size;
    char *ss_sp=nullptr;
};

void ctx_make(context *ctx, func_t coro_func, const void *arg);

}  // namespace coro
