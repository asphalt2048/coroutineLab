#pragma once

#include <coroutine>
#include <iterator>
#include <utility>

namespace coro {

template <typename Ref, typename Value = std::remove_cvref_t<Ref>>
class generator {
public:
    struct promise_type;
    using handle=std::coroutine_handle<promise_type>;
    struct promise_type{
        friend generator;
        public:
            union{
                promise_type* leaf_;
                promise_type* root_;
            };
            std::add_pointer_t<Ref> data;
            promise_type* parent_;


            promise_type():root_(this){}

            generator<Ref,Value> get_return_object(){return generator(this->co());}
            std::suspend_always initial_suspend() { return {}; }
            void unhandled_exception(){std::terminate();}
            void return_void(){}

            //final suspend
            struct final_awaitar{
                bool await_ready() noexcept {return false;}
                std::coroutine_handle<> await_suspend(handle h)noexcept{
                    auto& promise=h.promise();
                    auto parent=h.promise().parent_;
                    if(parent){
                        promise.root_->leaf_=parent;
                        return handle::from_promise(*parent);
                    }
                    else return std::noop_coroutine();
                }
                void await_resume() noexcept {}
            };
            final_awaitar final_suspend() noexcept { 
                return final_awaitar{};
            }


            std::suspend_always yield_value(Ref& value) noexcept {
                root_->data=std::addressof(value);
                return {};
            }

            std::suspend_always yield_value(Ref&& value) noexcept {
                root_->data=std::addressof(value);
                return {};
            }


            //co_yield a generator
            struct seq_awaitar{
                generator g_;

                explicit seq_awaitar(generator&& g):g_(std::move(g)){}
                bool await_ready() noexcept {return !g_.coro;}
                std::coroutine_handle<> await_suspend(handle h) noexcept {
                    auto& cur=h.promise();
                    auto& nested=g_.coro.promise();
                    auto& root=cur.root_;

                    nested.root_=root;
                    nested.parent_=&cur;
                    root->leaf_=&nested;

                    return g_.coro;
                }             
                void await_resume() noexcept {}
            };
            seq_awaitar yield_value(generator&& g) noexcept {
                return seq_awaitar{std::move(g)};
            }

            void resume(){handle::from_promise(*leaf_).resume();}

            auto co(){return handle::from_promise(*this);}

            void await_transfrom()=delete;
    };

    generator(generator&& other) noexcept :coro(std::exchange(other.coro,{})){}

    generator() noexcept = default;

    ~generator() noexcept {
        if(coro) coro.destroy();
    }

    generator& operator=(generator g) noexcept{
        std::swap(coro,g.coro);
        return *this;
    }

    struct empty{};

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

        friend bool operator==(const iterator& it,empty) noexcept {
            return (!it.coro_)||it.coro_.done();
        }
        friend bool operator!=(const iterator& it,empty) noexcept{
            return it.coro_&&(!it.coro_.done());
        }
        iterator& operator++(){
            coro_.promise().resume();
            return *this;
        }
        void operator++(int){
            (void)operator++();
        }
        reference operator*()const noexcept{
            return static_cast<reference>(*coro_.promise().data);
        }
        pointer operator->()const noexcept 
        requires std::is_reference_v<reference>{
            return std::addressof(operator*());
        }

    private:
        friend generator;

        explicit iterator(handle p) noexcept {coro_=p;}

        handle coro_;
    };

    iterator begin(){
        if(coro)
            coro.resume();
        return iterator{coro};
    }

    empty end(){
        return{};
    }

private:
    explicit generator(handle coro_in) noexcept {coro=coro_in;}

    handle coro;
};

}  // namespace coro
