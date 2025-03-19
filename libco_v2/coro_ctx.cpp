#include "coro_ctx.h"

#include <cstdint>
#include <cstring>

namespace coro {

void ctx_make(context* ctx, func_t coro_func, const void* arg) {
    // TODO: implement your code here
    char *sp=ctx->ss_sp+ctx->ss_size;
    sp=(char*)((unsigned long)sp&-16LL);
    ctx->rsp=sp-8;

    ctx->ret=(char *)coro_func;
    ctx->rdi=(char *)(arg);
}

}  // namespace coro
