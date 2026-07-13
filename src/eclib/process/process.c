/*
 * ECLib - E-comOS C Library
 * SLFD v10.2 - Process Management
 */
#include <slfd.h>
#include <string.h>

#define PROCESS_SERVICE_PID  2
#define PROCESS_CMD_NEW      0x2001
#define PROCESS_CMD_SETFLAGS 0x2002
#define PROCESS_CMD_START    0x2003
#define PROCESS_CMD_EXIT     0x2004
#define PROCESS_CMD_KILL     0x2005
#define PROCESS_CMD_SIGNAL   0x2006
#define PROCESS_CMD_GETPID   0x2007
#define PROCESS_CMD_GETPPID  0x2008
#define PROCESS_CMD_WAIT     0x2009
#define PROCESS_CMD_WHOAMI   0x200A
#define PROCESS_CMD_SETPRIV  0x200B

static int proc_call(uint32_t cmd, const void *req, size_t req_len,
                     void *resp, size_t resp_len) {
    ipc_message_t msg = {0};
    msg.type   = cmd;
    msg.target = PROCESS_SERVICE_PID;
    msg.size   = (req_len > IPC_MAX_DATA_SIZE) ? IPC_MAX_DATA_SIZE : (uint32_t)req_len;
    if (req && req_len) memcpy(msg.data, req, msg.size);
    if (sys_ipc_send(PROCESS_SERVICE_PID, &msg) != 0) return SLFD_ERR(EAGAIN);
    ipc_message_t reply = {0};
    if (sys_ipc_receive(&reply) != 0) return SLFD_ERR(EAGAIN);
    if (resp && resp_len) memcpy(resp, reply.data, resp_len);
    return 0;
}

pid_t proc_new(const char *args, uint32_t privilege_mask) {
    if (!args || (args[0] != 'n' && args[0] != 's') || args[1] != '\0')
        return SLFD_ERR(EINVAL);
    struct { char mode; uint32_t mask; } req = {args[0], privilege_mask};
    struct { int pid; int err; } resp = {0};
    if (proc_call(PROCESS_CMD_NEW, &req, sizeof(req), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    return resp.err ? SLFD_ERR(resp.err) : resp.pid;
}

int proc_setflags(pid_t pid, uint32_t flags) {
    struct { int pid; uint32_t flags; } req = {pid, flags};
    int err = 0;
    if (proc_call(PROCESS_CMD_SETFLAGS, &req, sizeof(req), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

int proc_start(pid_t pid, const char *path, const parg_t *parg) {
    if (!path) return SLFD_ERR(EINVAL);
    struct {
        int  pid;
        char path[256];
        int  argc;
        char args[1024];
    } req = {0};
    req.pid = pid;
    strncpy(req.path, path, sizeof(req.path) - 1);
    if (parg) {
        req.argc = parg->argc;
        char *p = req.args;
        for (int i = 0; parg->argv && parg->argv[i]; i++) {
            size_t l = strlen(parg->argv[i]);
            if ((size_t)(p - req.args) + l + 1 >= sizeof(req.args)) break;
            memcpy(p, parg->argv[i], l + 1);
            p += l + 1;
        }
    }
    int err = 0;
    if (proc_call(PROCESS_CMD_START, &req, sizeof(req), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

_Noreturn void proc_terminate(void) {
    ipc_message_t msg = {0};
    msg.type   = PROCESS_CMD_EXIT;
    msg.target = PROCESS_SERVICE_PID;
    sys_ipc_send(PROCESS_SERVICE_PID, &msg);
    while (1);
}

int proc_kill(pid_t pid) {
    int err = 0;
    if (proc_call(PROCESS_CMD_KILL, &pid, sizeof(pid), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

int proc_signal_send(pid_t pid, uint32_t signum) {
    struct { int pid; uint32_t sig; } req = {pid, signum};
    int err = 0;
    if (proc_call(PROCESS_CMD_SIGNAL, &req, sizeof(req), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

pid_t proc_getpid(void) {
    int resp = 0;
    proc_call(PROCESS_CMD_GETPID, 0, 0, &resp, sizeof(resp));
    return resp;
}

pid_t proc_getppid(void) {
    int resp = 0;
    proc_call(PROCESS_CMD_GETPPID, 0, 0, &resp, sizeof(resp));
    return resp;
}

pid_t waitpid(pid_t pid, int *status, int options) {
    struct { int pid; int options; } req = {pid, options};
    struct { int pid; int status; int err; } resp = {0};
    if (proc_call(PROCESS_CMD_WAIT, &req, sizeof(req), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    if (resp.err) return -resp.err;
    if (status) *status = resp.status;
    return resp.pid;
}

int whoami(void) {
    int resp = 0;
    if (proc_call(PROCESS_CMD_WHOAMI, 0, 0, &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    return resp;
}

int set_privilege(uint32_t mask) {
    int err = 0;
    if (proc_call(PROCESS_CMD_SETPRIV, &mask, sizeof(mask), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}
