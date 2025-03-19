#include <cassert>
#include <iostream>

#define CO_BEGIN         \
    if(!begin)           \
    {begin=true;start=0;}\
    switch(start){       \
    case 0:


#define CO_END            \
        case -1:          \
            end=true;     \
    }

#define CO_YIELD(a) start=__LINE__;return a;case __LINE__:;

#define CO_RETURN(a) end=true; start=-1; return a;

struct coroutine_base {
    int start=0;
    bool begin=false;
    bool end=false;
};

class fib : public coroutine_base {
private:
    /* TODO */
    int ret=0;
    int b=1;
public:
    // TODO: update below code when you implement
    // CO_BEGIN/CO_END/CO_YIELD/CO_RETURN
    int operator()() {
        CO_BEGIN
            while (1){
                CO_YIELD(ret)
                int temp=ret;
                ret=b;
                b=temp+b;
            }
        CO_END
        return -1;
    }
};

int main() {
    int ans[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
    fib foo;
    for (int i = 0; i < 10; i++)
        assert(foo() == ans[i]);
    std::cout << "libco_v3 test passed!" << std::endl;
}
