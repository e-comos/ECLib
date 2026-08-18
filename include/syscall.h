/*
 * ECLib - E-comOS C Library
 * syscall() - architecture-independent system call interface
 */
#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#define SYS_IPC_SEND 1
#define SYS_IPC_RECEIVE 2
#define SYS_THREAD_YIELD 3
#define SYS_ADDRESS_MAP 4
#define SYS_IRQ_WAIT 5
#define SYS_IRQ_GET_COUNT 6
#define SYS_IRQ_RESET_COUNT 7
#define SYS_THREAD_CREATE 8
#define SYS_MM_ALLOC_PAGES 9
#define SYS_MM_FREE_PAGES 10
#define SYS_SHM_OPEN 11
#define SYS_SHM_UNLINK 12
#define SYS_GETRLIMIT 15
#define SYS_SETRLIMIT 16
#define SYS_SLEEP_PRO 17
#define SYS_TIMER_CREATE 18
#define SYS_GET_MONOTIME 19
#define SYS_SETTIMEOFDAY 20
#define SYS_INFO_SYS 21
#define SYS_GETRANDOM 22
#define SYS_HLUD_REQUEST 23


long syscall(long number, ...);

#endif /* SYSCALL_H */
