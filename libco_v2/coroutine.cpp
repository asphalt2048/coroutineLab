#include "coroutine.h"

#include <cstring>
#include<iostream>

namespace coro {

static coroutine_env g_coro_env;

extern "C" {
extern void coro_ctx_swap(context*, context*) asm("coro_ctx_swap");
};

coroutine* create(func_t coro_func, void* arg, const coroutine_attr* attr) {
    coroutine_attr at;
    if (attr != nullptr) {
        at = *attr;
    }
    // TODO: implement your code here
    coroutine* co=new coroutine;
    co->ctx={0};
    co->arg=arg;
    co->coro_func=coro_func;

    if(at.sstack==nullptr){//not using share-stack
        if(at.stack_size>(1<<17))
            at.stack_size=1<<17;
        if(at.stack_size<(1<<13))
            at.stack_size=1<<13;
        at.stack_size=at.stack_size&(~(1<<12)+1);
        co->stk=new stack_mem(at.stack_size);
        co->stk_used=nullptr;
        co->ctx.ss_sp=co->stk->sp;
        co->ctx.ss_size=co->stk->stack_size;
    }
    else{//using share_stack
        at.stack_size=at.sstack->stack_size;
        if(at.stack_size>(1<<17)){
            int count=at.sstack->count;
            delete at.sstack;
            at.sstack=new share_stack(count,1<<17);
        }
        if(at.stack_size<(1<<13)){
            int count=at.sstack->count;
            delete at.sstack;
            at.sstack=new share_stack(count,1<<13);
        }

        co->stk=nullptr;
        co->stk_used=at.sstack->get_stackmem();
        co->ctx.ss_sp=co->stk_used->sp;
        co->ctx.ss_size=co->stk_used->stack_size;

    }
    return co;
}

void release(coroutine* co) {
    // TODO: implement your code here
    delete co;
}

void save_stack(coroutine* co) {
    // TODO: implement your code here
    if(co==nullptr) return;
    if(co->end) return;
    assert(co->size_has_used!=0);
    if(co->stk==nullptr)
        co->stk=new stack_mem(co->size_has_used);
    assert(co->stk!=nullptr);
    char* src=(co->ctx.ss_sp)+(co->ctx.ss_size)-(co->size_has_used);
    assert(co->size_has_used>0);
    memcpy(co->stk->sp,src,co->size_has_used);
    co->stk->stack_size=co->size_has_used;  
}

void swap(coroutine* curr, coroutine* pending) {
    // TODO: implement your code here
    if(curr->stk_used!=nullptr){
        int get_stk_size=0xff;
        long long size=(curr->ctx.ss_sp)+(curr->ctx.ss_size)-(char*)(&get_stk_size);
        size=(size+0x1<<4)&(-16LL);
        curr->size_has_used=size;
    }
    if(pending->stk_used!=nullptr&&pending->stk_used->last_user!=pending){
        save_stack(pending->stk_used->last_user);
        pending->stk_used->last_user=pending;
        if(pending->stk!=nullptr){
            char* dest=(pending->ctx.ss_sp)+(pending->ctx.ss_size)-(pending->size_has_used);
            char* src=pending->stk->sp;
            assert(pending->stk->stack_size>0);
            memcpy(dest,src,pending->stk->stack_size);
        }
    }
    coro_ctx_swap(&curr->ctx,&pending->ctx);
}

static void func_wrap(coroutine* co) {
    if (co->coro_func) {
        co->coro_func(co->arg);
    }
    co->end = true;
    yield(-1);
}

int resume(coroutine* co, int param) {
    // TODO: implement your code here
    coroutine *caller=g_coro_env.get_coro();
    if(co->started==false){
        ctx_make(&(co->ctx),(void(*)(void*))func_wrap,co);
        co->started=true;
    }
    co->data=param;
    g_coro_env.push(co);
    swap(caller,co);
    return co->data;
}

int yield(int ret) {
    // TODO: implement your code here
    coroutine *cur=g_coro_env.get_coro();
    g_coro_env.pop();
    coroutine *caller=g_coro_env.get_coro();

    int temp=cur->data;
    cur->data=ret;
    swap(cur,caller);

    return temp;
}

}  // namespace coro
