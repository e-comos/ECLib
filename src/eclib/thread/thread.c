/*
 * ECLib - E-comOS C Library
 * SLFD v10.2 - Thread Management
 */
#include <slfd.h>
#include <eclib/thread.h>
#include <string.h>

#define THREAD_SERVICE_PID  6
#define THREAD_CMD_CREATE   0x6001
#define THREAD_CMD_JOIN     0x6002
#define THREAD_CMD_EXIT     0x6003
#define THREAD_CMD_MUTEX_INIT    0x6010
#define THREAD_CMD_MUTEX_LOCK    0x6011
#define THREAD_CMD_MUTEX_UNLOCK  0x6012
#define THREAD_CMD_MUTEX_DESTROY 0x6013
#define THREAD_CMD_COND_INIT     0x6020
#define THREAD_CMD_COND_WAIT     0x6021
#define THREAD_CMD_COND_SIGNAL   0x6022
#define THREAD_CMD_COND_BCAST    0x6023
#define THREAD_CMD_COND_DESTROY  0x6024

static int thr_call(uint32_t cmd, const void *req, size_t req_len,
                    void *resp, size_t resp_len) {
    ipc_message_t msg = {0};
    msg.type   = cmd;
    msg.target = THREAD_SERVICE_PID;
    msg.size   = (req_len > IPC_MAX_DATA_SIZE) ? IPC_MAX_DATA_SIZE : (uint32_t)req_len;
    if (req && req_len) memcpy(msg.data, req, msg.size);
    if (sys_ipc_send(THREAD_SERVICE_PID, &msg) != 0) return SLFD_ERR(EAGAIN);
    ipc_message_t reply = {0};
    if (sys_ipc_receive(&reply) != 0) return SLFD_ERR(EAGAIN);
    if (resp && resp_len) memcpy(resp, reply.data, resp_len);
    return 0;
}

int thread_create(thread_id *tid, void *(*start)(void *), void *arg) {
    if (!tid || !start) return SLFD_ERR(EINVAL);
    struct { uintptr_t start; uintptr_t arg; } req = {
        (uintptr_t)start, (uintptr_t)arg
    };
    struct { uint32_t tid; int err; } resp = {0};
    if (thr_call(THREAD_CMD_CREATE, &req, sizeof(req), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    if (resp.err) return -resp.err;
    *tid = resp.tid;
    return 0;
}

int thread_join(thread_id tid, void **retval) {
    struct { uint32_t tid; } req = {tid};
    struct { uintptr_t ret; int err; } resp = {0};
    if (thr_call(THREAD_CMD_JOIN, &req, sizeof(req), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    if (resp.err) return -resp.err;
    if (retval) *retval = (void *)resp.ret;
    return 0;
}

_Noreturn void thread_exit(void *retval) {
    uintptr_t r = (uintptr_t)retval;
    ipc_message_t msg = {0};
    msg.type   = THREAD_CMD_EXIT;
    msg.target = THREAD_SERVICE_PID;
    msg.size   = sizeof(r);
    memcpy(msg.data, &r, sizeof(r));
    sys_ipc_send(THREAD_SERVICE_PID, &msg);
    while (1);
}

int mutex_init(mutex_t *m)    { if (!m) return SLFD_ERR(EINVAL); m->_state = 0; return 0; }
int mutex_lock(mutex_t *m)    { if (!m) return SLFD_ERR(EINVAL); return thr_call(THREAD_CMD_MUTEX_LOCK,    m, sizeof(*m), m, sizeof(*m)); }
int mutex_unlock(mutex_t *m)  { if (!m) return SLFD_ERR(EINVAL); return thr_call(THREAD_CMD_MUTEX_UNLOCK,  m, sizeof(*m), m, sizeof(*m)); }
int mutex_destroy(mutex_t *m) { if (!m) return SLFD_ERR(EINVAL); m->_state = 0; return 0; }

int cond_init(cond_t *c)      { if (!c) return SLFD_ERR(EINVAL); c->_state = 0; return 0; }
int cond_destroy(cond_t *c)   { if (!c) return SLFD_ERR(EINVAL); c->_state = 0; return 0; }

int cond_wait(cond_t *c, mutex_t *m) {
    if (!c || !m) return SLFD_ERR(EINVAL);
    struct { uint32_t cstate; uint32_t mstate; } req = {c->_state, m->_state};
    struct { uint32_t cstate; uint32_t mstate; int err; } resp = {0};
    if (thr_call(THREAD_CMD_COND_WAIT, &req, sizeof(req), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    c->_state = resp.cstate;
    m->_state = resp.mstate;
    return resp.err ? SLFD_ERR(resp.err) : 0;
}

int cond_signal(cond_t *c) {
    if (!c) return SLFD_ERR(EINVAL);
    int err = 0;
    if (thr_call(THREAD_CMD_COND_SIGNAL, c, sizeof(*c), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

int cond_broadcast(cond_t *c) {
    if (!c) return SLFD_ERR(EINVAL);
    int err = 0;
    if (thr_call(THREAD_CMD_COND_BCAST, c, sizeof(*c), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}
