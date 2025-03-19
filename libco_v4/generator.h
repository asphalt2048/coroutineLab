#pragma once

#include <coroutine>
#include <iterator>
#include <utility>

namespace coro {

template <typename Ref, typename Value = std::remove_cvref_t<Ref>>
class generator {
public:
    // TODO: implement promise_type
    struct promise_type;
    using handle = std::coroutine_handle<promise_type>;
    struct promise_type{
        public: 
            Value data;

            promise_type():data(0){}

            generator<Ref,Value> get_return_object(){return generator(this->co());} 
            std::suspend_always initial_suspend() { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            void unhandled_exception() { std::terminate(); }
            void return_void(){}
            std::suspend_always yield_value(Value value) noexcept{
                data = value;
                return {};
            }
            auto co() { return handle::from_promise(*this); }
            Value& getvalue(){return data;}
    };

    generator() noexcept = default;

    ~generator() noexcept {
        /* TODO */
        if(coro) coro.destroy();
    }
    
    class empty{};
    
    class iterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = Value;
        using reference = Ref;
        using pointer = std::add_pointer_t<Ref>;

        iterator() noexcept = default;
        iterator(const iterator&) = delete;
        iterator(iterator&& o) {
            std::swap(coro_, o.coro_);
        }

        iterator& operator=(iterator&& o) {
            std::swap(coro_, o.coro_);
            return *this;
        }

        ~iterator() {}

        // TODO: implement operator== and operator!=
        bool operator==(const empty&) const noexcept{
            return !coro_||coro_.done();
        }
        bool operator!=(const empty&) const noexcept{
            return coro_&&!coro_.done();
        }
        // TODO: implement operator++ and operator++(int)
        iterator& operator++(){
            coro_.resume();
            return *this;
        }
        void operator++(int){
            coro_.resume();
        }
        // TODO: implement operator* and operator->
        Value& operator*(){
            return coro_.promise().getvalue();
        }
        Value* operator->(){
            return &(coro_.promise().getvalue());
        }

    private:
        friend generator;

        // TODO: implement iterator constructor
        // hint: maybe you need to a promise handle
        explicit iterator(handle p) noexcept {coro_=p;}

        // TODO: add member variables you need
        handle coro_;
    };

    // TODO: implement begin() and end() member functions
    iterator begin(){
        auto iter=iterator(coro);
        ++iter;
        return iter;
    }

    empty end(){
        return{};
    }

private:
    // TODO: implement generator constructor
    explicit generator(handle coro_in) noexcept {coro=coro_in;}

    // TODO: add member variables you need
    handle coro;
};

}  // namespace coro
