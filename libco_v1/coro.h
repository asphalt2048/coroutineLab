#pragma once

#include <ucontext.h>
#include <cassert>

namespace coro {

class coroutine;
using func_t = void (*)(void*);

coroutine* create(func_t func, void* args);
void release(coroutine* co);
int resume(coroutine* co, int param = 0);
int yield(int ret = 0);

struct coroutine {
    bool started = false;//the flag that indicates whether the corotine has been invoked before
    bool end = false;

    func_t coro_func = nullptr;
    void* args = nullptr;

    // TODO: add member variables you need
    char stack[1024*1024];
    ucontext_t ctx = {0};
    int state=0;//-1 indecates the end of coroutine, that to say, yield(-1) means the coro has ended

    coroutine(func_t func, void* args) : coro_func(func), args(args) {
        //actually don't need to write anything
    }

    ~coroutine() {
    }
};

class coroutine_env {
private:
    // TODO: add member variables you need
    coroutine* stack[10]={nullptr}; //the stack stores coroutines
    int top=-1;
public:
    coroutine_env() {
        coroutine* main_coro=create(nullptr,nullptr);//push main's coroutine in stack,using create
        push(main_coro);
    }
    coroutine* get_coro(int idx=-1) {
        if(idx>=0&&idx<=top)
            return stack[idx];
        else
            return stack[top];
    }
    void push(coroutine* co) {
        assert(top<9);  
        stack[++top]=co;
    }
    void pop() {
        stack[top]=nullptr;
        top--;
    }
};

}  // namespace coro