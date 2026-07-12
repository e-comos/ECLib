// C99 Standard 7.5 Errors <errno.h>
// Defines error codes and the errno global variable for error reporting
// Compliant with C99 standard; requires errno.c for errno variable definition
#ifndef ERRNO_H
#define ERRNO_H
// -------------------------- Global Error Variable Declaration (C99 Required) --------------------------
// errno: global variable to store the last error code from system calls or library functions
// - 0 indicates no error; non-zero indicates a specific error (defined below)
// - Modified only when a function fails; not reset to 0 on successful function calls
extern int errno;

// -------------------------- C99 Standard Error Codes (Required, Minimal Set) --------------------------
// Basic standard error codes for E-comOS v0.1 (expand as needed for full C99 compliance)
#define EOK        0   // No error (success, non-standard but convenient for E-comOS)
#define EPERM      1   // Operation not permitted (insufficient privilege for the requested operation)
#define ENOENT     2   // No such file or directory (requested resource does not exist)
#define ESRCH      3   // No such process (target pid does not exist)
#define EINTR      4   // Interrupted system call
#define EIO        5   // Input/output error (failed to read/write from/to device/resource)
#define ENXIO      6   // No such device or address
#define E2BIG      7   // Argument list too long
#define ENOEXEC    8   // Exec format error (executable image is invalid or unsupported)
#define EBADF      9   // Bad file descriptor (invalid or closed file descriptor used)
#define ECHILD     10  // No child process (waitpid target has no matching child)
#define EAGAIN     11  // Resource temporarily unavailable (try again later)
#define ENOMEM     12  // Out of memory (insufficient memory to complete operation)
#define EACCES     13  // Permission denied (insufficient privileges to access resource)
#define EFAULT     14  // Bad address (invalid pointer passed to function)
#define EINVAL     15  // Invalid argument (invalid parameter passed to function)

// -------------------------- E-comOS Extended Error Codes (Custom, for Kernel/Drivers) --------------------------
// Additional error codes tailored for E-comOS kernel and device drivers (expand as needed)
#define EIPC       100 // IPC communication failure (inter-process communication error)
#define EDEVICE    101 // Device error (hardware device malfunction or unavailable)
#define EPROTO     102 // Protocol error (invalid protocol sequence or response)
#define EOVERFLOW  103 // Overflow error (numeric value too large for target type)
#define EUNIMP     104 // Unimplemented function (function not yet implemented in E-comOS)
#endif /* ERRNO_H */