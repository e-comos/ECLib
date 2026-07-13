/*
 * ECLib - E-comOS C Library
 * SLFD Specification v10.2 - <slfd/thread.h>
 */
#ifndef SYS_THREAD_H
#define SYS_THREAD_H

#include <stdint.h>

typedef uint32_t thread_id;

int          thread_create(thread_id *tid, void *(*start)(void *), void *arg);
int          thread_join(thread_id tid, void **retval);
_Noreturn void thread_exit(void *retval);

typedef struct { uint32_t _state; } mutex_t;
int mutex_init(mutex_t *mutex);
int mutex_lock(mutex_t *mutex);
int mutex_unlock(mutex_t *mutex);
int mutex_destroy(mutex_t *mutex);

typedef struct { uint32_t _state; } cond_t;
int cond_init(cond_t *cond);
int cond_wait(cond_t *cond, mutex_t *mutex);
int cond_signal(cond_t *cond);
int cond_broadcast(cond_t *cond);
int cond_destroy(cond_t *cond);

#endif /* SYS_THREAD_H */
