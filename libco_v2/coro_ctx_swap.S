.text

.globl coro_ctx_swap
coro_ctx_swap:
leaq 8(%rsp),%rax
leaq 112(%rdi),%rsp
pushq %rax
pushq %r15
pushq %r14
pushq %r13
pushq %r12
pushq %rbx
pushq %rbp
pushq -8(%rax)
pushq %r9
pushq %r8
pushq %rcx
pushq %rdx
pushq %rsi
pushq %rdi

movq %rsi, %rsp
popq %rdi
popq %rsi
popq %rdx
popq %rcx
popq %r8
popq %r9
popq %rax
popq %rbp
popq %rbx
popq %r12
popq %r13
popq %r14
popq %r15
popq %rsp
jmp *%rax
ret