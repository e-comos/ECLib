/*
 * ECLib - E-comOS C Library
 * SLFD v10.2 - IPC
 */
#include <slfd.h>
#include <syscall.h>
#include <string.h>

#define _SYS_IPC_SEND    1
#define _SYS_IPC_RECEIVE 2

int sys_ipc_send(thread_id target, ipc_message_t *msg) {
    return (int)syscall(_SYS_IPC_SEND, (long)target, (long)msg, 0, 0, 0, 0);
}

int sys_ipc_receive(ipc_message_t *msg) {
    return (int)syscall(_SYS_IPC_RECEIVE, (long)msg, 0, 0, 0, 0, 0);
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
