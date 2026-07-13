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
#include <eclib/ipc_message.h>
#include <string.h>
#include <syscall.h>

#define SYS_IPC_SEND        1001
#define SYS_IPC_RECEIVE     1002
#define SYS_IPC_DO_NOT_KILL 1003
#define SYS_IPC_BROADCAST   1004
#define SYS_GETPID          1010

int ipc_send_msg(uint32_t type, uint32_t flags, uint32_t receiver_pid,
                 uint32_t data_len, const void *data) {
    ipc_message_t msg = {0};
    msg.type        = type;
    msg.sender_pid  = (uint32_t)syscall(SYS_GETPID);
    msg.receiver_pid = receiver_pid;
    msg.data_len    = (data_len > 256) ? 256 : data_len;
    msg.flags       = flags;
    if (data && data_len > 0)
        memcpy(msg.data, data, msg.data_len);
    return (int)syscall(SYS_IPC_SEND, (long)&msg);
}

int ipc_receive_msg(ipc_message_t *msg, int timeout_ms) {
    if (!msg) return -1;
    return (int)syscall(SYS_IPC_RECEIVE, (long)msg, (long)timeout_ms);
}

int ipc_broadcast_msg(uint32_t type, uint32_t flags, uint32_t data_len,
                      const void *data) {
    ipc_message_t msg = {0};
    msg.type         = type;
    msg.sender_pid   = (uint32_t)syscall(SYS_GETPID);
    msg.receiver_pid = 0xFFFFFFFF;
    msg.data_len     = (data_len > 256) ? 256 : data_len;
    msg.flags        = flags;
    if (data && data_len > 0)
        memcpy(msg.data, data, msg.data_len);
    return (int)syscall(SYS_IPC_BROADCAST, (long)&msg);
}

int ipc_do_not_kill_sub(void) {
    int ret = (int)syscall(SYS_IPC_DO_NOT_KILL, 0L);
    if (ret == 0)
        ipc_broadcast_msg(IPC_MSG_DO_NOT_KILL, 0, 0, 0);
    return ret;
}

int ipc_do_not_kill_sub_emergency_ok(void) {
    int ret = (int)syscall(SYS_IPC_DO_NOT_KILL, 1L);
    if (ret == 0)
        ipc_broadcast_msg(IPC_MSG_DO_NOT_KILL, 1, 0, 0);
    return ret;
}
