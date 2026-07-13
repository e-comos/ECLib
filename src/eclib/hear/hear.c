/*
 * ECLib - E-comOS C Library
 * SLFD v10.2 - hear interface (event multiplexing)
 */
#include <slfd.h>
#include <string.h>

#define HEAR_SERVICE_PID 5
#define HEAR_CMD_ADD     0x5001
#define HEAR_CMD_REMOVE  0x5002
#define HEAR_CMD_FILTER  0x5003
#define HEAR_CMD_WAIT    0x5004

static int hear_call(uint32_t cmd, const void *req, size_t req_len,
                     void *resp, size_t resp_len) {
    ipc_message_t msg = {0};
    msg.type   = cmd;
    msg.target = HEAR_SERVICE_PID;
    msg.size   = (req_len > IPC_MAX_DATA_SIZE) ? IPC_MAX_DATA_SIZE : (uint32_t)req_len;
    if (req && req_len) memcpy(msg.data, req, msg.size);
    if (sys_ipc_send(HEAR_SERVICE_PID, &msg) != 0) return SLFD_ERR(EAGAIN);
    ipc_message_t reply = {0};
    if (sys_ipc_receive(&reply) != 0) return SLFD_ERR(EAGAIN);
    if (resp && resp_len) memcpy(resp, reply.data, resp_len);
    return 0;
}

int hear_add(fid_t fid, int events) {
    struct { int fid; int events; } req = {fid, events};
    int err = 0;
    if (hear_call(HEAR_CMD_ADD, &req, sizeof(req), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

int hear_remove(fid_t fid) {
    int err = 0;
    if (hear_call(HEAR_CMD_REMOVE, &fid, sizeof(fid), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

int hear_filter(fid_t fid, int new_events) {
    struct { int fid; int events; } req = {fid, new_events};
    int err = 0;
    if (hear_call(HEAR_CMD_FILTER, &req, sizeof(req), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

int hear_wait(void *events_out, int max_events, int timeout_ms) {
    if (!events_out || max_events <= 0) return SLFD_ERR(EINVAL);
    struct { int max; int timeout_ms; } req = {max_events, timeout_ms};
    /* response: count + array of hear_event */
    struct {
        int              count;
        int              err;
        struct hear_event ev[64];
    } resp = {0};
    if (hear_call(HEAR_CMD_WAIT, &req, sizeof(req), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    if (resp.err) return -resp.err;
    int n = resp.count < max_events ? resp.count : max_events;
    memcpy(events_out, resp.ev, (size_t)n * sizeof(struct hear_event));
    return n;
}
