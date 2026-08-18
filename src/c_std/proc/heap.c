#include <stddef.h>
#include <stdint.h>
#include <string.h>

/* SLFD core memory primitives */
void *page_malloc(uint32_t count);
void page_mfree(void *pages, uint32_t count);

typedef struct header {
    size_t size;
    uint32_t page_count;
} header_t;

void *malloc(size_t size) {
    if (size == 0) return NULL;
    
    size_t total_size = size + sizeof(header_t);
    uint32_t pages_needed = (uint32_t)((total_size + 4095) / 4096);
    
    header_t *block = (header_t *)page_malloc(pages_needed);
    if (!block) return NULL;
    
    block->size = size;
    block->page_count = pages_needed;
    
    return (void *)(block + 1);
}

void free(void *ptr) {
    if (!ptr) return;
    
    header_t *block = ((header_t *)ptr) - 1;
    page_mfree(block, block->page_count);
}

void *calloc(size_t nmemb, size_t size) {
    if (nmemb == 0 || size == 0) return NULL;
    
    size_t total = nmemb * size;
    if (total / nmemb != size) return NULL; // Overflow check
    
    /* SLFD page_malloc guarantees zero-filled physical pages */
    return malloc(total);
}

void *realloc(void *ptr, size_t size) {
    if (!ptr) return malloc(size);
    if (size == 0) {
        free(ptr);
        return NULL;
    }
    
    header_t *block = ((header_t *)ptr) - 1;
    size_t old_size = block->size;
    
    if (size <= old_size) return ptr;
    
    void *new_ptr = malloc(size);
    if (!new_ptr) return NULL;
    
    memcpy(new_ptr, ptr, old_size);
    free(ptr);
    
    return new_ptr;
}
