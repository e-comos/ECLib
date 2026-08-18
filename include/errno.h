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
#define ENOBLK     15  // Invalid argument (invalid parameter passed to function)
#define EBUSY	   16  // Device or resource busy.
#define EEXIST 	   17  // File exists.
#define EXDEV	   18  // Cross-device link
#define ENODEV	   19  // No this device
#define ENOTDIR	   20  // It's not a directory
#define EISDIR	   21  // It's a directory
#define EINVAL	   22  // Invalid argument
#define ENFILE	   23  // File table overflow
#define EMFILE	   24  // Too many files you open
#define ENOTTY	   25  // Inappropriate I/O control.
#define ETXTBSY	   26  // Text file is busy now.
#define EFBIG 	   27  // Your file is too fat(large).
#define ENOSPC	   28  // No space left on this device
#define ESPIPE	   29  // Illegal seek
#define EROFS	   30  // Read-only file system
#define EMLINK	   31  // Too many links here
#define EPIPE	   32  // Pipe broken
#define EDOM	   33  // Math argument out of domain
#define ERANGE	   34  // Result too large
#define EDEADLK	   35  // Deadlock avoided
#define ENAMETOOLONG 36 // File name is too long like this error code
#define ENOLCK	   37  // No locks available.
#define ENOSYS     38  // This function is un-implemented in this system for now.
#define ENOTEMPTY  39  // That directory not empty
#define ELOOP	   40  // Too many symbolic links.
#define EWOULDBLOCK 41 // Same as EAGAIN
#define ENOMSG     42  // No message of desired type.
#define EIDRM	   43  // Identifier removed.
#define ETIMEDOUT  44  // Connection timed ou
#define ERESTART   45  // Restart syscall (internal)
#define EMSGSIZE   46  // Message too long
#define EADDRINUS  47  // Address already in use
#define ECONNREFUSED 48 // Connection refused.
#define ECONNRESET 49  // Connection reset by peer.
#define ENOTCONN   50  // Socket not connected
#define ETIME	   51  // Timer expired
#define ECANCELED  52  // Operation is canceled


// -------------------------- E-comOS Extended Error Codes (Custom, for Kernel/Drivers) --------------------------
// Additional error codes tailored for E-comOS kernel and device drivers (expand as needed)
#define EIPC       100 // IPC communication failure (inter-process communication error)
#define EDEVICE    101 // Device error (hardware device malfunction or unavailable)
#define EPROTO     102 // Protocol error (invalid protocol sequence or response)
#define EOVERFLOW  103 // Overflow error (numeric value too large for target type)
#define EUNIMP     104 // Unimplemented function (function not yet implemented in E-comOS)
#endif /* ERRNO_H */
