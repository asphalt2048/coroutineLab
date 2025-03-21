#include <coroutine>
#include <functional>
#include <queue>
#include <thread>
#include<iostream>

namespace coro {

static std::queue<std::function<bool()>> task_queue;
static std::queue<std::coroutine_handle<>> my_queue;


struct sleep {
    sleep(int n_ms) : delay{n_ms} {}

    std::chrono::milliseconds delay;


    bool await_ready()noexcept{return false;}
    void await_suspend(std::coroutine_handle<> h) noexcept {
        std::this_thread::sleep_for(delay);
        my_queue.push(h);
    };
    void await_resume() noexcept {}
    sleep(){}
};

struct Task {
public:
    struct promise_type;
    using handle=std::coroutine_handle<promise_type>;
    struct promise_type{
        Task get_return_object()noexcept{
            return Task{};
        }
        void return_void()noexcept{}
        std::suspend_never initial_suspend()noexcept{return {};}
        void unhandled_exception(){std::terminate();}
        std::suspend_always final_suspend() noexcept {return {};}
    };
};

void wait_task_queue_empty() {
    while (!my_queue.empty())
    {
        std::coroutine_handle<> h=my_queue.front();
        my_queue.pop();
        h.resume();
    }   
}

}  // namespace coro
