/*
 * ECLib - E-comOS C Library
 * syscall() - architecture-independent system call interface
 */
#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

long syscall(long number, ...);

#endif /* SYSCALL_H */
