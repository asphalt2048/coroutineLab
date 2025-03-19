#pragma once

#include <cassert>
#include <cstdlib>
#include<malloc.h>

#include "coro_ctx.h"

namespace coro {

struct coroutine;
struct coroutine_attr;

coroutine *create(func_t coro_func, void *arg, const coroutine_attr *attr = nullptr);
void release(coroutine *co);

int resume(coroutine *co, int param = 0);
int yield(int ret = 0);

struct stack_mem {
    int stack_size = 0;
    // TODO: add member variables you need
    char *sp=nullptr;

    coroutine* last_user=nullptr;

    stack_mem(size_t size) : stack_size(size) {
        // TODO: implement your code here
        sp=new char[size]();
    }

    ~stack_mem() {
        // TODO: implement your code here
        last_user=nullptr;
        if(sp!=nullptr)
            delete sp;
        sp=nullptr;
    }
};

struct share_stack {
    // TODO: add member variables you need
    int count = 0;
    int idx=-1;
    int stack_size = 0;
    stack_mem **stack_array = nullptr;

    share_stack(int count, size_t stack_size)
        : count(count), stack_size(stack_size) {
        // TODO: implement your code here
        assert(count>=1);
        stack_array=new stack_mem*[count];
        for(int i=0;i<count;i++)
            stack_array[i]=new stack_mem(stack_size);
    }
    ~share_stack() {
        // TODO: implement your code here
        for(int i=0;i<count;i++)
            delete stack_array[i];
        delete[] stack_array;
    }
    stack_mem *get_stackmem() {
        // TODO: implement your code here
        assert(stack_array!=nullptr&&count!=0);
        idx=(idx+1)%count;
        return stack_array[idx];
    } 
};

struct coroutine {
    bool started = false;
    bool end = false;

    stack_mem* stk_used=nullptr;

    func_t coro_func = nullptr;
    void *arg = nullptr;

    // TODO: add member variables you need
    stack_mem *stk=nullptr;
    context ctx = {0};
    int data=0;
    long long size_has_used=0;

    ~coroutine() {
        // TODO: implement your code here
        if(stk_used!=nullptr){
            stk_used->last_user=nullptr;
            stk_used=nullptr;
        }
        if(stk!=nullptr&&stk->sp!=nullptr) 
            delete stk;
    }
};

struct coroutine_attr {
    int stack_size = 128 * 1024;
    share_stack *sstack = nullptr;
};

class coroutine_env {
private:
    // TODO: add member variables you need
    coroutine* coroutines[100]={nullptr};
    int top=-1;

public:
    // TODO: add member variables you need

public:
    coroutine_env() {
        // TODO: implement your code here
        coroutine *co=create(nullptr,nullptr,nullptr);
        push(co);
    }

    coroutine *get_coro(int idx=-1) {
        // TODO: implement your code here
        if(idx>=0&&idx<=top)
            return coroutines[idx];
        else
            return coroutines[top];
    }

    void pop() {
        // TODO: implement your code here
        coroutines[top]=nullptr;
        top--;
    }

    void push(coroutine *co) {
        // TODO: implement your code here
        top++;
        coroutines[top]=co;
    }
};

}  // namespace coro
