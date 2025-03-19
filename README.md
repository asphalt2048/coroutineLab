# coroutine-lab

这是一份迭代开发的协程调度器。

分为多个版本。
- v1-v2是有栈协程，其中：
    1. v1利用linux的ucontext_t实现
    2. v2抛开ucontext，自己实现完整的协程管理。包括上下文的保存、切换，栈空间的分配，共享栈的实现等等
- v3-v5是无栈协程，主要基于c++ 23的协程支持。其完成极大程度上借助了[这篇文章](https://lewissbaker.github.io/)

每个版本中，`main.cpp`都是用于测试协程实现的正确性的。