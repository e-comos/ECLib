/*
 * ECLib - E-comOS C Library
 * SLFD v10.2 - Memory Management
 */
#include <slfd.h>
#include <syscall.h>

#define _SYS_MM_ALLOC_PAGES 9
#define _SYS_MM_FREE_PAGES  10
#define _SYS_SHM_OPEN       11
#define _SYS_SHM_UNLINK     12

void *page_malloc(uint32_t count) {
    if (count == 0) return 0;
    return (void *)(uintptr_t)syscall(_SYS_MM_ALLOC_PAGES, (long)count, 0, 0, 0, 0, 0);
}

void page_mfree(void *pages, uint32_t count) {
    if (!pages || count == 0) return;
    syscall(_SYS_MM_FREE_PAGES, (long)pages, (long)count, 0, 0, 0, 0);
}

fid_t shm_open(const char *name, int oflags, uint32_t mode) {
    if (!name) return SLFD_ERR(EINVAL);
    return (fid_t)syscall(_SYS_SHM_OPEN, (long)name, (long)oflags, (long)mode, 0, 0, 0);
}

int shm_unlink(const char *name) {
    if (!name) return SLFD_ERR(EINVAL);
    return (int)syscall(_SYS_SHM_UNLINK, (long)name, 0, 0, 0, 0, 0);
}
