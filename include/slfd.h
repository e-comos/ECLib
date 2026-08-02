/*
 * ECLib - E-comOS C Library
 * SLFD Specification v10.2 - Primary header <slfd.h>
 */
#ifndef SLFD_H
#define SLFD_H

#include <stdint.h>
#include <stddef.h>
#include <errno.h>
#include <eclib/ula.h>

/* Set errno and return a negative error code in one step */
#define SLFD_ERR(e) (errno = (e), -(e))

/* ------------------------------------------------------------------ */
/* Types                                                                */
/* ------------------------------------------------------------------ */
typedef int      fid_t;
typedef int      pid_t;
typedef long     off_t;
typedef long     ssize_t;
typedef uint32_t thread_id;

typedef struct {
    int    argc;
    char **argv;
    char **envp;
} parg_t;

struct stat {
    size_t   st_size;
    uint32_t st_attr_mask;
};

struct utsname {
    char sysname[32];
    char nodename[32];
    char release[32];
    char version[32];
    char machine[32];
};

struct rlimit {
    unsigned long rlim_cur;
    unsigned long rlim_max;
};

#define IPC_MAX_DATA_SIZE 4096
typedef struct ipc_message {
    uint32_t type;
    uint32_t source;
    uint32_t target;
    uint32_t timestamp;
    uint32_t size;
    uint32_t sequence;
    uint8_t  data[IPC_MAX_DATA_SIZE];
} ipc_message_t;

/* ------------------------------------------------------------------ */
/* Resource flags                                                       */
/* ------------------------------------------------------------------ */
#define PROC_IO_READ    (1<<0)
#define PROC_IO_WRITE   (1<<1)
#define PROC_NETWORK    (1<<2)
#define PROC_MEMORY     (1<<3)

/* ------------------------------------------------------------------ */
/* Signal numbers                                                       */
/* ------------------------------------------------------------------ */
#define PROCSIG_HUP     1
#define PROCSIG_INT     2
#define PROCSIG_QUIT    3
#define PROCSIG_ILL     4
#define PROCSIG_TRAP    5
#define PROCSIG_ABRT    6
#define PROCSIG_BUS     7
#define PROCSIG_FPE     8
#define PROCSIG_KILL    9
#define PROCSIG_USR1    10
#define PROCSIG_SEGV    11
#define PROCSIG_USR2    12
#define PROCSIG_PIPE    13
#define PROCSIG_ALRM    14
#define PROCSIG_TERM    15
#define PROCSIG_CHLD    17
#define PROCSIG_STOP    18
#define PROCSIG_CONT    19
#define PROCSIG_TSTP    20

/* ------------------------------------------------------------------ */
/* I/O seek constants                                                   */
/* ------------------------------------------------------------------ */
#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

/* ------------------------------------------------------------------ */
/* Mount flags                                                          */
/* ------------------------------------------------------------------ */
#define MS_RDONLY       1
#define MS_NOSUID       2
#define MS_NODEV        4
#define MS_NOEXEC       8
#define MS_SYNCHRONOUS  16
#define MS_REMOUNT      32
#define MS_BIND         1024
#define MS_MOVE         2048
#define MS_REC          4096
#define MS_PRIVATE      131072
#define MS_SLAVE        262144
#define MS_SHARED       524288

#define UMASK_DEFAULT   0022

/* ------------------------------------------------------------------ */
/* Error codes (negative values returned on failure)                   */
/* ------------------------------------------------------------------ */
#define EPERM           1
#define ENOENT          2
#define ESRCH           3
#define EINTR           4
#define EIO             5
#define ENXIO           6
#define E2BIG           7
#define ENOEXEC         8
#define EBADF           9
#define ECHILD          10
#define EAGAIN          11
#define ENOMEM          12
#define EACCES          13
#define EFAULT          14
#define ENOTBLK         15
#define EBUSY           16
#define EEXIST          17
#define EXDEV           18
#define ENODEV          19
#define ENOTDIR         20
#define EISDIR          21
#define EINVAL          22
#define ENFILE          23
#define EMFILE          24
#define ENOTTY          25
#define ETXTBSY         26
#define EFBIG           27
#define ENOSPC          28
#define ESPIPE          29
#define EROFS           30
#define EMLINK          31
#define EPIPE           32
#define EDOM            33
#define ERANGE          34
#define EDEADLK         35
#define ENAMETOOLONG    36
#define ENOLCK          37
#define ENOSYS          38
#define ENOTEMPTY       39
#define ELOOP           40
#define EWOULDBLOCK     41
#define ENOMSG          42
#define EIDRM           43
#define ETIMEDOUT       44
#define ERESTART        45
#define EMSGSIZE        46
#define EADDRINUSE      47
#define ECONNREFUSED    48
#define ECONNRESET      49
#define ENOTCONN        50
#define ETIME           51
#define ECANCELED       52

/* ------------------------------------------------------------------ */
/* Process Management                                                   */
/* ------------------------------------------------------------------ */
#define WNOHANG 1

pid_t proc_new(const char *args, uint32_t privilege_mask);
int   proc_setflags(pid_t pid, uint32_t flags);
int   proc_start(pid_t pid, const char *path, const parg_t *parg);
_Noreturn void proc_terminate(void);
int   proc_kill(pid_t pid);
int   proc_signal_send(pid_t pid, uint32_t signum);
pid_t proc_getpid(void);
pid_t proc_getppid(void);
pid_t waitpid(pid_t pid, int *status, int options);
int   whoami(void);
int   set_privilege(uint32_t mask);

/* ------------------------------------------------------------------ */
/* Memory Management                                                    */
/* ------------------------------------------------------------------ */
void *page_malloc(uint32_t count);
void  page_mfree(void *pages, uint32_t count);
fid_t shm_open(const char *name, int oflags, uint32_t mode);
int   shm_unlink(const char *name);

/* ------------------------------------------------------------------ */
/* File and Device I/O                                                  */
/* ------------------------------------------------------------------ */
fid_t  open(const char *type, const char *mode, const char *addr);
int    close(const char *type, fid_t fid);
ssize_t read(const char *type, fid_t fid, void *buf, size_t count);
ssize_t write(const char *type, fid_t fid, const void *buf, size_t count);
off_t  io_seek(const char *type, fid_t fid, off_t offset, int whence);
fid_t  pipe(pid_t to_who);
int    godir(const char *path);
int    godir_byfid(fid_t fid);
int    getwd(char *buf, size_t size);
fid_t  cpfid(fid_t fid);
int    cpfid2(fid_t oldfd, fid_t newfd);
int    set_flags(fid_t fid, int flags);
int    umask(int mask);

/* ------------------------------------------------------------------ */
/* Network Communications                                               */
/* ------------------------------------------------------------------ */
fid_t   net_connect(const char *protocol, const char *address);
fid_t   net_listen(const char *protocol, const char *address);
fid_t   net_accept(fid_t listen_fid);
ssize_t net_send(fid_t fid, const void *buf, size_t count);
ssize_t net_recv(fid_t fid, void *buf, size_t count);
int     net_disconnect(fid_t fid);

/* ------------------------------------------------------------------ */
/* IPC                                                                  */
/* ------------------------------------------------------------------ */
int sys_ipc_send(thread_id target, ipc_message_t *msg);
int sys_ipc_receive(ipc_message_t *msg);
int ipc_send_msg(uint32_t type, uint32_t flags, uint32_t receiver_pid,
                 uint32_t data_len, const void *data);
int ipc_receive_msg(ipc_message_t *msg, int timeout_ms);

/* ------------------------------------------------------------------ */
/* hear interface (event multiplexing)                                  */
/* ------------------------------------------------------------------ */
#define HEAR_IN     (1<<0)
#define HEAR_OUT    (1<<1)
#define HEAR_IPC    (1<<2)
#define HEAR_SIGNAL (1<<3)
#define HEAR_PROC   (1<<4)

struct hear_event {
    fid_t  handle;
    int    type;
    void  *userdata;
};

int hear_add(fid_t fid, int events);
int hear_remove(fid_t fid);
int hear_filter(fid_t fid, int new_events);
int hear_wait(void *events_out, int max_events, int timeout_ms);

/* ------------------------------------------------------------------ */
/* Thread Management                                                    */
/* ------------------------------------------------------------------ */
#include <eclib/thread.h>

/* ------------------------------------------------------------------ */
/* Extended File System Operations (Section 10.3)                       */
/* ------------------------------------------------------------------ */
struct fs_dirent {
    char d_name[256];
    uint32_t d_type;
};

int stat(const char *path, struct stat *buf);
int mkdir(const char *path, uint32_t attr_mask);
int rmdir(const char *path);
fid_t fs_opendir(const char *path);
int fs_readdir(fid_t dir, struct fs_dirent *ent);
int fs_closedir(fid_t dir);
int fs_unlink(const char *path);
int fs_rename(const char *oldpath, const char *newpath);

#endif /* SLFD_H */
