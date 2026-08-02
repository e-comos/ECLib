#ifndef ECLIB_ERROR_H
#define ECLIB_ERROR_H

#include <errno.h>
#include <stddef.h>

typedef int eclib_err_t;

#define ECLIB_OK 0
#define ECLIB_ECLIB_INVALID_PARAMETER EINVAL
#define ECLIB_ECLIB_CANNOT_FIND_MODULE -ENOSYS
#define ECLIB_FILE_INVALID (-1)

static inline eclib_err_t set_last_err(eclib_err_t err) {
    errno = (err < 0) ? -err : err;
    return err;
}

#endif
