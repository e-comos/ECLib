/*
 * ECLib - E-comOS C Library
 * SLFD v10.2 - File and Device I/O
 */
#include <slfd.h>
#include <string.h>

#define FS_SERVICE_PID  3
#define FS_CMD_OPEN     0x3001
#define FS_CMD_CLOSE    0x3002
#define FS_CMD_READ     0x3003
#define FS_CMD_WRITE    0x3004
#define FS_CMD_SEEK     0x3005
#define FS_CMD_PIPE     0x3006
#define FS_CMD_GODIR    0x3007
#define FS_CMD_GODIRF   0x3008
#define FS_CMD_GETWD    0x3009
#define FS_CMD_CPFID    0x300A
#define FS_CMD_CPFID2   0x300B
#define FS_CMD_SETFLAGS 0x300C
#define FS_CMD_UMASK    0x300D

static int fs_call(uint32_t cmd, const void *req, size_t req_len,
                   void *resp, size_t resp_len) {
    ipc_message_t msg = {0};
    msg.type   = cmd;
    msg.target = FS_SERVICE_PID;
    msg.size   = (req_len > IPC_MAX_DATA_SIZE) ? IPC_MAX_DATA_SIZE : (uint32_t)req_len;
    if (req && req_len) memcpy(msg.data, req, msg.size);
    if (sys_ipc_send(FS_SERVICE_PID, &msg) != 0) return SLFD_ERR(EAGAIN);
    ipc_message_t reply = {0};
    if (sys_ipc_receive(&reply) != 0) return SLFD_ERR(EAGAIN);
    if (resp && resp_len) memcpy(resp, reply.data, resp_len);
    return 0;
}

fid_t open(const char *type, const char *mode, const char *addr) {
    if (!type || !mode || !addr) return SLFD_ERR(EINVAL);
    struct { char type[4]; char mode[4]; char addr[256]; } req = {0};
    strncpy(req.type, type, sizeof(req.type) - 1);
    strncpy(req.mode, mode, sizeof(req.mode) - 1);
    strncpy(req.addr, addr, sizeof(req.addr) - 1);
    struct { int fid; int err; } resp = {0};
    if (fs_call(FS_CMD_OPEN, &req, sizeof(req), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    return resp.err ? SLFD_ERR(resp.err) : resp.fid;
}

int close(const char *type, fid_t fid) {
    if (!type) return SLFD_ERR(EINVAL);
    struct { char type[4]; int fid; } req = {0};
    strncpy(req.type, type, sizeof(req.type) - 1);
    req.fid = fid;
    int err = 0;
    if (fs_call(FS_CMD_CLOSE, &req, sizeof(req), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

ssize_t read(const char *type, fid_t fid, void *buf, size_t count) {
    if (!type || !buf || count == 0) return SLFD_ERR(EINVAL);
    struct { char type[4]; int fid; size_t count; } req = {0};
    strncpy(req.type, type, sizeof(req.type) - 1);
    req.fid   = fid;
    req.count = count;
    struct { ssize_t n; int err; uint8_t data[IPC_MAX_DATA_SIZE - 16]; } resp = {0};
    if (fs_call(FS_CMD_READ, &req, sizeof(req), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    if (resp.err) return -resp.err;
    size_t actual = (size_t)resp.n < count ? (size_t)resp.n : count;
    memcpy(buf, resp.data, actual);
    return resp.n;
}

ssize_t write(const char *type, fid_t fid, const void *buf, size_t count) {
    if (!type || !buf || count == 0) return SLFD_ERR(EINVAL);
    struct {
        char    type[4];
        int     fid;
        size_t  count;
        uint8_t data[IPC_MAX_DATA_SIZE - 16];
    } req = {0};
    strncpy(req.type, type, sizeof(req.type) - 1);
    req.fid   = fid;
    req.count = count;
    size_t chunk = count < sizeof(req.data) ? count : sizeof(req.data);
    memcpy(req.data, buf, chunk);
    struct { ssize_t n; int err; } resp = {0};
    if (fs_call(FS_CMD_WRITE, &req, sizeof(req), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    return resp.err ? SLFD_ERR(resp.err) : resp.n;
}

off_t io_seek(const char *type, fid_t fid, off_t offset, int whence) {
    if (!type) return SLFD_ERR(EINVAL);
    struct { char type[4]; int fid; long offset; int whence; } req = {0};
    strncpy(req.type, type, sizeof(req.type) - 1);
    req.fid    = fid;
    req.offset = offset;
    req.whence = whence;
    struct { long pos; int err; } resp = {0};
    if (fs_call(FS_CMD_SEEK, &req, sizeof(req), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    return resp.err ? SLFD_ERR(resp.err) : resp.pos;
}

fid_t pipe(pid_t to_who) {
    struct { int fid; int err; } resp = {0};
    if (fs_call(FS_CMD_PIPE, &to_who, sizeof(to_who), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    return resp.err ? SLFD_ERR(resp.err) : resp.fid;
}

int godir(const char *path) {
    if (!path) return SLFD_ERR(EINVAL);
    char req[256] = {0};
    strncpy(req, path, sizeof(req) - 1);
    int err = 0;
    if (fs_call(FS_CMD_GODIR, req, sizeof(req), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

int godir_byfid(fid_t fid) {
    int err = 0;
    if (fs_call(FS_CMD_GODIRF, &fid, sizeof(fid), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

int getwd(char *buf, size_t size) {
    if (!buf || size == 0) return SLFD_ERR(EINVAL);
    struct { char path[256]; int err; } resp = {0};
    if (fs_call(FS_CMD_GETWD, 0, 0, &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    if (resp.err) return -resp.err;
    strncpy(buf, resp.path, size - 1);
    buf[size - 1] = '\0';
    return 0;
}

fid_t cpfid(fid_t fid) {
    struct { int fid; int err; } resp = {0};
    if (fs_call(FS_CMD_CPFID, &fid, sizeof(fid), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    return resp.err ? SLFD_ERR(resp.err) : resp.fid;
}

int cpfid2(fid_t oldfd, fid_t newfd) {
    struct { int oldfd; int newfd; } req = {oldfd, newfd};
    int err = 0;
    if (fs_call(FS_CMD_CPFID2, &req, sizeof(req), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

int set_flags(fid_t fid, int flags) {
    struct { int fid; int flags; } req = {fid, flags};
    int err = 0;
    if (fs_call(FS_CMD_SETFLAGS, &req, sizeof(req), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

int umask(int mask) {
    int old = 0;
    fs_call(FS_CMD_UMASK, &mask, sizeof(mask), &old, sizeof(old));
    return old;
}
