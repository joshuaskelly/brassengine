#ifndef THREAD_POOL_H
#define THREAD_POOL_H

typedef struct thread_pool thread_pool_t;

typedef void (thread_work_func_t)(void*);

thread_pool_t* thread_pool_new(int count);
void thread_pool_free(thread_pool_t* pool);
void thread_pool_add_work(thread_pool_t* pool, thread_work_func_t*, void* arg);
void thread_pool_wait(thread_pool_t* pool);

#endif
