/*
 * ECLib - E-comOS C Library
 * SLFD v10.2 - Memory Management
 */
#include <slfd.h>

/* Kernel primitives (Appendix D) */
extern void *sys_mm_alloc_pages(uint32_t count);
extern void  sys_mm_free_pages(void *pages, uint32_t count);
extern fid_t sys_shm_open(const char *name, int oflags, uint32_t mode);
extern int   sys_shm_unlink(const char *name);

void *page_malloc(uint32_t count) {
    if (count == 0) return 0;
    return sys_mm_alloc_pages(count);
}

void page_mfree(void *pages, uint32_t count) {
    if (!pages || count == 0) return;
    sys_mm_free_pages(pages, count);
}

fid_t shm_open(const char *name, int oflags, uint32_t mode) {
    if (!name) return SLFD_ERR(EINVAL);
    return sys_shm_open(name, oflags, mode);
}

int shm_unlink(const char *name) {
    if (!name) return SLFD_ERR(EINVAL);
    return sys_shm_unlink(name);
}
