/*
 * ECLib - E-comOS C Library
 * Copyright (C) 2025 E-comOS Kernel Mode Team & Saladin5101
 *
 * This file is part of ECLib.
 * ECLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 */
#ifndef ECLIB_PROCESS_H
#define ECLIB_PROCESS_H

#include <stdint.h>

#define ECLIB_WNOHANG 1

#define ECLIB_WIFEXITED(status) (((status) & 0xFF) == 0)
#define ECLIB_WEXITSTATUS(status) (((status) >> 8) & 0xFF)

int proc_new(const char* mode, uint32_t privilege_mask);
int proc_setflags(int pid, uint32_t flags);
int proc_start(int pid, const char* path, char* const argv[], char* const envp[]);
_Noreturn void proc_terminate(void);
int proc_kill(int pid);
int proc_signal_send(int pid, uint32_t signum);
int proc_getpid(void);
int proc_getppid(void);
int waitpid(int pid, int* status, int options);
int whoami(void);
int set_privilege(uint32_t mask);

#endif