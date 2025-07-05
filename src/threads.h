#ifndef THREADS_H
#define THREADS_H

#include <stdlib.h>

typedef struct thread thread_t;

thread_t* threads_thread_new(void* (function)(void*), void* args);

void threads_thread_free(thread_t* thread);

void threads_thread_detatch(thread_t* thread);

void* threads_thread_join(thread_t* thread);

void threads_thread_exit(void* result);

typedef struct thread_lock thread_lock_t;

thread_lock_t* threads_thread_lock_new(void);

void threads_thread_lock_free(thread_lock_t* lock);

void threads_lock_lock(thread_lock_t* lock);

void threads_lock_unlock(thread_lock_t* lock);

typedef struct thread_condition thread_condition_t;

thread_condition_t* threads_thread_condition_new(void);

void threads_thread_condition_free(thread_condition_t* condition);

void threads_thread_condition_wait(thread_condition_t* condition, thread_lock_t* lock);

void threads_thread_condition_alert(thread_condition_t* condition);

typedef struct thread_pool thread_pool_t;

thread_pool_t* threads_thread_pool_new(size_t count);

void threads_thread_pool_add_work(thread_pool_t* thread_pool, void(function)(void*), void* arg);

void threads_thread_pool_wait(thread_pool_t* thread_pool);

void threads_thread_pool_free(thread_pool_t* thread_pool);

#endif
