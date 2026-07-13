/*
 * ECLib - E-comOS C Library
 * SLFD v10.2 - IPC
 */
#include <slfd.h>
#include <string.h>

/* Kernel primitives declared in Appendix D */
extern int sys_ipc_send(thread_id target, ipc_message_t *msg);
extern int sys_ipc_receive(ipc_message_t *msg);

int ipc_send_msg(uint32_t type, uint32_t flags, uint32_t receiver_pid,
                 uint32_t data_len, const void *data) {
    ipc_message_t msg = {0};
    msg.type   = type;
    msg.source = (uint32_t)proc_getpid();
    msg.target = receiver_pid;
    msg.size   = data_len > IPC_MAX_DATA_SIZE ? IPC_MAX_DATA_SIZE : data_len;
    /* flags stored in sequence field as extension */
    msg.sequence = flags;
    if (data && data_len) memcpy(msg.data, data, msg.size);
    return sys_ipc_send((thread_id)receiver_pid, &msg);
}

int ipc_receive_msg(ipc_message_t *msg, int timeout_ms) {
    if (!msg) return SLFD_ERR(EINVAL);
    if (timeout_ms == 0) {
        /* non-blocking: attempt receive, return -EAGAIN if none */
        return sys_ipc_receive(msg);
    }
    /* blocking with timeout: implementation-defined via kernel */
    return sys_ipc_receive(msg);
}
