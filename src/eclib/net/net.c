/*
 * ECLib - E-comOS C Library
 * SLFD v10.2 - Network Communications
 */
#include <slfd.h>
#include <string.h>

#define NET_SERVICE_PID  4
#define NET_CMD_CONNECT  0x4001
#define NET_CMD_LISTEN   0x4002
#define NET_CMD_ACCEPT   0x4003
#define NET_CMD_SEND     0x4004
#define NET_CMD_RECV     0x4005
#define NET_CMD_DISC     0x4006

static int net_call(uint32_t cmd, const void *req, size_t req_len,
                    void *resp, size_t resp_len) {
    ipc_message_t msg = {0};
    msg.type   = cmd;
    msg.target = NET_SERVICE_PID;
    msg.size   = (req_len > IPC_MAX_DATA_SIZE) ? IPC_MAX_DATA_SIZE : (uint32_t)req_len;
    if (req && req_len) memcpy(msg.data, req, msg.size);
    if (sys_ipc_send(NET_SERVICE_PID, &msg) != 0) return SLFD_ERR(EAGAIN);
    ipc_message_t reply = {0};
    if (sys_ipc_receive(&reply) != 0) return SLFD_ERR(EAGAIN);
    if (resp && resp_len) memcpy(resp, reply.data, resp_len);
    return 0;
}

fid_t net_connect(const char *protocol, const char *address) {
    if (!protocol || !address) return SLFD_ERR(EINVAL);
    struct { char proto[8]; char addr[256]; } req = {0};
    strncpy(req.proto, protocol, sizeof(req.proto) - 1);
    strncpy(req.addr,  address,  sizeof(req.addr)  - 1);
    struct { int fid; int err; } resp = {0};
    if (net_call(NET_CMD_CONNECT, &req, sizeof(req), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    return resp.err ? SLFD_ERR(resp.err) : resp.fid;
}

fid_t net_listen(const char *protocol, const char *address) {
    if (!protocol || !address) return SLFD_ERR(EINVAL);
    struct { char proto[8]; char addr[256]; } req = {0};
    strncpy(req.proto, protocol, sizeof(req.proto) - 1);
    strncpy(req.addr,  address,  sizeof(req.addr)  - 1);
    struct { int fid; int err; } resp = {0};
    if (net_call(NET_CMD_LISTEN, &req, sizeof(req), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    return resp.err ? SLFD_ERR(resp.err) : resp.fid;
}

fid_t net_accept(fid_t listen_fid) {
    struct { int fid; int err; } resp = {0};
    if (net_call(NET_CMD_ACCEPT, &listen_fid, sizeof(listen_fid), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    return resp.err ? SLFD_ERR(resp.err) : resp.fid;
}

ssize_t net_send(fid_t fid, const void *buf, size_t count) {
    if (!buf || count == 0) return SLFD_ERR(EINVAL);
    struct {
        int     fid;
        size_t  count;
        uint8_t data[IPC_MAX_DATA_SIZE - 16];
    } req = {0};
    req.fid   = fid;
    req.count = count;
    size_t chunk = count < sizeof(req.data) ? count : sizeof(req.data);
    memcpy(req.data, buf, chunk);
    struct { ssize_t n; int err; } resp = {0};
    if (net_call(NET_CMD_SEND, &req, sizeof(req), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    return resp.err ? SLFD_ERR(resp.err) : resp.n;
}

ssize_t net_recv(fid_t fid, void *buf, size_t count) {
    if (!buf || count == 0) return SLFD_ERR(EINVAL);
    struct { int fid; size_t count; } req = {fid, count};
    struct { ssize_t n; int err; uint8_t data[IPC_MAX_DATA_SIZE - 16]; } resp = {0};
    if (net_call(NET_CMD_RECV, &req, sizeof(req), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    if (resp.err) return -resp.err;
    size_t actual = (size_t)resp.n < count ? (size_t)resp.n : count;
    memcpy(buf, resp.data, actual);
    return resp.n;
}

int net_disconnect(fid_t fid) {
    int err = 0;
    if (net_call(NET_CMD_DISC, &fid, sizeof(fid), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}
