/*
 * ECLib - E-comOS C Library
 * Copyright (C) 2025, 2026 Junhao Ding(@Saladin5101)
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 * 
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <slfd.h>
#include <syscall.h>
#include <string.h>

#define _SYS_IPC_SEND    1
#define _SYS_IPC_RECEIVE 2

int sys_ipc_send(thread_id target, ipc_message_t *msg) {
    return (int)syscall(_SYS_IPC_SEND, (long)target, (long)msg);
}

int sys_ipc_receive(ipc_message_t *msg) {
    return (int)syscall(_SYS_IPC_RECEIVE, (long)msg);
}

int ipc_send_msg(uint32_t type, uint32_t flags, uint32_t receiver_pid,
                 uint32_t data_len, const void *data) {
    ipc_message_t msg = {0};
    msg.type     = type;
    msg.source   = (uint32_t)proc_getpid();
    msg.target   = receiver_pid;
    msg.size     = data_len > IPC_MAX_DATA_SIZE ? IPC_MAX_DATA_SIZE : data_len;
    msg.sequence = flags;
    if (data && data_len) memcpy(msg.data, data, msg.size);
    return sys_ipc_send((thread_id)receiver_pid, &msg);
}


int ipc_receive_msg(ipc_message_t *msg, int timeout_ms) {
    if (!msg) return SLFD_ERR(EINVAL);
    (void)timeout_ms;
    return sys_ipc_receive(msg);
}
