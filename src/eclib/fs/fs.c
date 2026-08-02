/*
 * ECLib - E-comOS C Library
 * SLFD v10.3 - Extended File System Operations
 */
#include <slfd.h>
#include <string.h>

#define FS_SERVICE_PID  3
#define FS_CMD_STAT     0x3010
#define FS_CMD_MKDIR    0x3011
#define FS_CMD_RMDIR    0x3012
#define FS_CMD_OPENDIR  0x3013
#define FS_CMD_READDIR  0x3014
#define FS_CMD_CLOSEDIR 0x3015
#define FS_CMD_UNLINK   0x3016
#define FS_CMD_RENAME   0x3017

static int fs_ext_call(uint32_t cmd, const void *req, size_t req_len,
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

int stat(const char *path, struct stat *buf) {
    if (!path || !buf) return SLFD_ERR(EINVAL);
    struct { char path[256]; struct stat st; int err; } req = {0};
    strncpy(req.path, path, sizeof(req.path) - 1);
    struct { struct stat st; int err; } resp = {0};
    if (fs_ext_call(FS_CMD_STAT, &req, sizeof(req), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    *buf = resp.st;
    return resp.err ? -resp.err : 0;
}

int mkdir(const char *path, uint32_t attr_mask) {
    if (!path) return SLFD_ERR(EINVAL);
    struct { char path[256]; uint32_t attr; } req = {0};
    strncpy(req.path, path, sizeof(req.path) - 1);
    req.attr = attr_mask;
    int err = 0;
    if (fs_ext_call(FS_CMD_MKDIR, &req, sizeof(req), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

int rmdir(const char *path) {
    if (!path) return SLFD_ERR(EINVAL);
    char req[256] = {0};
    strncpy(req, path, sizeof(req) - 1);
    int err = 0;
    if (fs_ext_call(FS_CMD_RMDIR, req, sizeof(req), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

fid_t fs_opendir(const char *path) {
    if (!path) return SLFD_ERR(EINVAL);
    char req[256] = {0};
    strncpy(req, path, sizeof(req) - 1);
    struct { int fid; int err; } resp = {0};
    if (fs_ext_call(FS_CMD_OPENDIR, req, sizeof(req), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    return resp.err ? SLFD_ERR(resp.err) : resp.fid;
}

int fs_readdir(fid_t dir, struct fs_dirent *ent) {
    if (!ent) return SLFD_ERR(EINVAL);
    struct { int dir; struct fs_dirent e; int err; int more; } resp = {0};
    if (fs_ext_call(FS_CMD_READDIR, &dir, sizeof(dir), &resp, sizeof(resp)) != 0)
        return SLFD_ERR(EAGAIN);
    if (resp.err) return -resp.err;
    if (resp.more == 0) return 0;
    *ent = resp.e;
    return 1;
}

int fs_closedir(fid_t dir) {
    int err = 0;
    if (fs_ext_call(FS_CMD_CLOSEDIR, &dir, sizeof(dir), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

int fs_unlink(const char *path) {
    if (!path) return SLFD_ERR(EINVAL);
    char req[256] = {0};
    strncpy(req, path, sizeof(req) - 1);
    int err = 0;
    if (fs_ext_call(FS_CMD_UNLINK, req, sizeof(req), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}

int fs_rename(const char *oldpath, const char *newpath) {
    if (!oldpath || !newpath) return SLFD_ERR(EINVAL);
    struct { char old[256]; char new[256]; } req = {0};
    strncpy(req.old, oldpath, sizeof(req.old) - 1);
    strncpy(req.new, newpath, sizeof(req.new) - 1);
    int err = 0;
    if (fs_ext_call(FS_CMD_RENAME, &req, sizeof(req), &err, sizeof(err)) != 0)
        return SLFD_ERR(EAGAIN);
    return err ? -err : 0;
}