#include "coro.h"

namespace coro {

static coroutine_env g_coro_env;

coroutine* create(func_t func, void* args) {
    // TODO: implement your code here
    coroutine* co=new coroutine(func,args);
    return co;
}

void release(coroutine* co) {
    // TODO: implement your code here
    delete co;
}

static void func_wrap(coroutine* co) {
    if (co->coro_func) {
        co->coro_func(co->args);
    }
    //as you can see here, when a corotine run to completion, the "end" flag will be setted to true
    //and we will use yield(-1) to return to caller or resumer
    co->end = true;
    yield(-1);
}

int resume(coroutine* co, int param) {
    // TODO: implement your code here
    if(co->state==-1)//ended
        return -1;

    coroutine* caller=g_coro_env.get_coro();
    //if start for the first time, initialize
    if(!co->started){
        getcontext(&co->ctx);
        co->ctx.uc_link=&(caller->ctx);
        co->ctx.uc_stack.ss_sp=co->stack;
        co->ctx.uc_stack.ss_size=1024*1024;
        co->ctx.uc_stack.ss_flags=0;
        makecontext(&co->ctx,(void(*)())func_wrap,1,co);

        co->started=true;
    }
    co->state=param;
    g_coro_env.push(co);
    swapcontext(&caller->ctx,&co->ctx);
    
    return co->state;
}

int yield(int ret) {
    // TODO: implement your code here
    coroutine* cur=g_coro_env.get_coro();
    g_coro_env.pop();
    coroutine* caller=g_coro_env.get_coro();

    int temp=cur->state;
    cur->state=ret;
    swapcontext(&cur->ctx,&caller->ctx);

    return temp;
}  

}  // namespace coro