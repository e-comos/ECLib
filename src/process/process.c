/*
 * ECLib - E-comOS C Library
 * Copyright (C) 2025 E-comOS Kernel Mode Team & Saladin5101
 */
#include <eclib/process.h>
#include <eclib/ipc_message.h>
#include <eclib/service.h>
#include <string.h>
#include <eclib/error.h>
#include <errno.h>

#define PROCESS_SERVICE_NAME "process_service"
#define PROCESS_CMD_NEW       0x2001
#define PROCESS_CMD_SETFLAGS  0x2002
#define PROCESS_CMD_START     0x2003
#define PROCESS_CMD_WAIT      0x2004
#define PROCESS_CMD_EXIT      0x2005
#define PROCESS_CMD_GETPPID   0x2006
#define PROCESS_CMD_GETPID    0x2007
#define PROCESS_CMD_KILL      0x2008
#define PROCESS_CMD_SIGNAL    0x2009
#define PROCESS_CMD_WHOAMI    0x200A
#define PROCESS_CMD_SETPRIV   0x200B

typedef struct {
    char mode[2]; // "n" or "s"
    uint32_t privilege_mask;
} new_req_t;

typedef struct {
    int pid;
    int err; // errno.h code, 0 (EOK) on success
} new_resp_t;

typedef struct {
    int pid;
    uint32_t flags;
} setflags_req_t;

typedef struct {
    int err;
} setflags_resp_t;

typedef struct {
    int pid;
    char path[256];
    char argv_data[1024];
    char envp_data[1024];
} start_req_t;

typedef struct {
    int err;
} start_resp_t;

typedef struct {
    int pid;
    int options;
} wait_req_t;

typedef struct {
    int pid;
    int status;
    int err;
} wait_resp_t;

typedef struct {
    int pid;
} kill_req_t;

typedef struct {
    int err;
} kill_resp_t;

typedef struct {
    int pid;
    uint32_t signum;
} signal_req_t;

typedef struct {
    int err;
} signal_resp_t;

typedef struct {
    uint32_t mask;
} setpriv_req_t;

typedef struct {
    int err;
} setpriv_resp_t;

// Creates a new process object (SLFD 5.1). mode is "n" (new, no memory
// image) or "s" (clone, inherits a copy of the caller's memory image).
// The process does not run until proc_start is called. Returns pid on
// success, negative errno.h code on failure.
int proc_new(const char* mode, uint32_t privilege_mask) {
    if (!mode || (mode[0] != 'n' && mode[0] != 's') || mode[1] != '\0') {
        return -EINVAL;
    }

    uint32_t service_pid = eclib_service_lookup(PROCESS_SERVICE_NAME);
    if (!service_pid) return -EIPC;

    new_req_t req;
    req.mode[0] = mode[0];
    req.mode[1] = '\0';
    req.privilege_mask = privilege_mask;

    new_resp_t resp;
    size_t resp_len = sizeof(resp);

    if (ipc_call_sync(service_pid, PROCESS_CMD_NEW, &req, sizeof(req),
                      &resp, &resp_len, 5000) != 0) {
        return -EAGAIN;
    }

    if (resp.err != EOK) return -resp.err;
    return resp.pid;
}

// Sets resource flags (PROC_IO_READ, PROC_IO_WRITE, PROC_NETWORK,
// PROC_MEMORY, ...) for a process created by proc_new. Must be called
// before proc_start; flags cannot change after the process has started.
int proc_setflags(int pid, uint32_t flags) {
    uint32_t service_pid = eclib_service_lookup(PROCESS_SERVICE_NAME);
    if (!service_pid) return -EIPC;

    setflags_req_t req = {pid, flags};
    setflags_resp_t resp;
    size_t resp_len = sizeof(resp);

    if (ipc_call_sync(service_pid, PROCESS_CMD_SETFLAGS, &req, sizeof(req),
                      &resp, &resp_len, 5000) != 0) {
        return -EAGAIN;
    }

    return resp.err == EOK ? 0 : -resp.err;
}

// Loads the executable at `path` into the process created by proc_new
// and starts it running with the given argv/envp. Can only be called
// once per pid (SLFD 5.3.1: "must not have been started before").
int proc_start(int pid, const char* path, char* const argv[], char* const envp[]) {
    if (!path) return -EINVAL;

    uint32_t service_pid = eclib_service_lookup(PROCESS_SERVICE_NAME);
    if (!service_pid) return -EIPC;

    start_req_t req = {0};
    req.pid = pid;
    strncpy(req.path, path, 255);

    char* p = req.argv_data;
    for (int i = 0; argv && argv[i]; i++) {
        size_t len = strlen(argv[i]);
        if ((size_t)(p - req.argv_data) + len + 1 < sizeof(req.argv_data)) {
            memcpy(p, argv[i], len + 1);
            p += len + 1;
        }
    }

    p = req.envp_data;
    for (int i = 0; envp && envp[i]; i++) {
        size_t len = strlen(envp[i]);
        if ((size_t)(p - req.envp_data) + len + 1 < sizeof(req.envp_data)) {
            memcpy(p, envp[i], len + 1);
            p += len + 1;
        }
    }

    start_resp_t resp;
    size_t resp_len = sizeof(resp);

    if (ipc_call_sync(service_pid, PROCESS_CMD_START, &req, sizeof(req),
                      &resp, &resp_len, 5000) != 0) {
        return -EAGAIN;
    }

    return resp.err == EOK ? 0 : -resp.err;
}

// Terminates the calling process immediately. Does not return.
_Noreturn void proc_terminate(void) {
    uint32_t service_pid = eclib_service_lookup(PROCESS_SERVICE_NAME);
    if (service_pid) {
        int status = 0;
        ipc_send_msg(service_pid, 0, PROCESS_CMD_EXIT, sizeof(status), &status);
    }
    while (1);
}

// Sends PROCSIG_KILL to pid. Requires LV_HIGHEST (SLFD Appendix A.3).
int proc_kill(int pid) {
    uint32_t service_pid = eclib_service_lookup(PROCESS_SERVICE_NAME);
    if (!service_pid) return -EIPC;

    kill_req_t req = {pid};
    kill_resp_t resp;
    size_t resp_len = sizeof(resp);

    if (ipc_call_sync(service_pid, PROCESS_CMD_KILL, &req, sizeof(req),
                      &resp, &resp_len, 5000) != 0) {
        return -EAGAIN;
    }

    return resp.err == EOK ? 0 : -resp.err;
}

// Sends an arbitrary signal (PROCSIG_* from eclib/signal.h) to pid.
int proc_signal_send(int pid, uint32_t signum) {
    uint32_t service_pid = eclib_service_lookup(PROCESS_SERVICE_NAME);
    if (!service_pid) return -EIPC;

    signal_req_t req = {pid, signum};
    signal_resp_t resp;
    size_t resp_len = sizeof(resp);

    if (ipc_call_sync(service_pid, PROCESS_CMD_SIGNAL, &req, sizeof(req),
                      &resp, &resp_len, 5000) != 0) {
        return -EAGAIN;
    }

    return resp.err == EOK ? 0 : -resp.err;
}

int proc_getpid(void) {
    uint32_t service_pid = eclib_service_lookup(PROCESS_SERVICE_NAME);
    if (!service_pid) return 0;

    int req = 0;
    int resp = 0;
    size_t resp_len = sizeof(resp);

    ipc_call_sync(service_pid, PROCESS_CMD_GETPID, &req, sizeof(req),
                  &resp, &resp_len, 5000);

    return resp;
}

int proc_getppid(void) {
    uint32_t service_pid = eclib_service_lookup(PROCESS_SERVICE_NAME);
    if (!service_pid) return 0;

    int req = 0;
    int resp = 0;
    size_t resp_len = sizeof(resp);

    ipc_call_sync(service_pid, PROCESS_CMD_GETPPID, &req, sizeof(req),
                  &resp, &resp_len, 5000);

    return resp;
}

// Only ECLIB_WNOHANG is a supported option (SLFD 5.9).
int waitpid(int pid, int* status, int options) {
    uint32_t service_pid = eclib_service_lookup(PROCESS_SERVICE_NAME);
    if (!service_pid) return -EIPC;

    wait_req_t req = {pid, options};
    wait_resp_t resp;
    size_t resp_len = sizeof(resp);

    if (ipc_call_sync(service_pid, PROCESS_CMD_WAIT, &req, sizeof(req),
                      &resp, &resp_len, 5000) != 0) {
        return -EAGAIN;
    }

    if (resp.err != EOK) return -resp.err;
    if (status) *status = resp.status;
    return resp.pid;
}

// Returns the caller's current ULA level (LV_HIGHEST..LV_GETTING).
int whoami(void) {
    uint32_t service_pid = eclib_service_lookup(PROCESS_SERVICE_NAME);
    if (!service_pid) return -EIPC;

    int req = 0;
    int resp = 0;
    size_t resp_len = sizeof(resp);

    if (ipc_call_sync(service_pid, PROCESS_CMD_WHOAMI, &req, sizeof(req),
                      &resp, &resp_len, 5000) != 0) {
        return -EAGAIN;
    }

    return resp;
}

// Sets the caller's privilege mask. Requires hlud authorization.
int set_privilege(uint32_t mask) {
    uint32_t service_pid = eclib_service_lookup(PROCESS_SERVICE_NAME);
    if (!service_pid) return -EIPC;

    setpriv_req_t req = {mask};
    setpriv_resp_t resp;
    size_t resp_len = sizeof(resp);

    if (ipc_call_sync(service_pid, PROCESS_CMD_SETPRIV, &req, sizeof(req),
                      &resp, &resp_len, 5000) != 0) {
        return -EAGAIN;
    }

    return resp.err == EOK ? 0 : -resp.err;
}