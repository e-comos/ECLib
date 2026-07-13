/*
 * ECLib - E-comOS C Library
 * syscall() implementation — aarch64 and x86_64
 *
 * Calling convention (E-comOS):
 *   syscall number in first arg register, up to 6 args follow.
 *   Return value in result register.
 */
#include <syscall.h>
#include <stdarg.h>

long syscall(long number, ...) {
    va_list ap;
    va_start(ap, number);
    long a1 = va_arg(ap, long);
    long a2 = va_arg(ap, long);
    long a3 = va_arg(ap, long);
    long a4 = va_arg(ap, long);
    long a5 = va_arg(ap, long);
    long a6 = va_arg(ap, long);
    va_end(ap);

    long ret;

#if defined(__aarch64__)
    register long x8  __asm__("x8")  = number;
    register long x0  __asm__("x0")  = a1;
    register long x1  __asm__("x1")  = a2;
    register long x2  __asm__("x2")  = a3;
    register long x3  __asm__("x3")  = a4;
    register long x4  __asm__("x4")  = a5;
    register long x5  __asm__("x5")  = a6;
    __asm__ volatile (
        "svc #0"
        : "=r"(x0)
        : "r"(x8), "r"(x0), "r"(x1), "r"(x2), "r"(x3), "r"(x4), "r"(x5)
        : "memory", "cc"
    );
    ret = x0;

#elif defined(__x86_64__)
    register long rax __asm__("rax") = number;
    register long rdi __asm__("rdi") = a1;
    register long rsi __asm__("rsi") = a2;
    register long rdx __asm__("rdx") = a3;
    register long r10 __asm__("r10") = a4;
    register long r8  __asm__("r8")  = a5;
    register long r9  __asm__("r9")  = a6;
    __asm__ volatile (
        "syscall"
        : "=r"(rax)
        : "r"(rax), "r"(rdi), "r"(rsi), "r"(rdx), "r"(r10), "r"(r8), "r"(r9)
        : "memory", "rcx", "r11", "cc"
    );
    ret = rax;

#else
#error "Unsupported architecture: add syscall() asm for your target"
#endif

    return ret;
}
