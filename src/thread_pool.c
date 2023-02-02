#include <pthread.h>
#include <stdlib.h>

#include "list.h"
#include "thread_pool.h"

typedef struct {
    thread_work_func_t* func;
    void* args;
} thread_pool_work_t;

static thread_pool_work_t* thread_pool_work_new() {

}

static void thread_pool_work_free(thread_pool_work_t* work) {

}

struct thread_pool {
    list_t* work;
    pthread_mutex_t mutex;
    int count;
};

thread_pool_t* thread_pool_new(int count) {
    thread_pool_t* pool = (thread_pool_t*)malloc(sizeof(thread_pool_t));

    pool->work = list_new();
    pool->count = count;

    pthread_mutex_init(&(pool->mutex), NULL);

    return pool;
}

void thread_pool_free(thread_pool_t* pool) {
    pthread_mutex_destroy(&(pool->mutex));
    list_free(pool->work);
    free(pool);
}

void thread_pool_add_work(thread_pool_t* pool, thread_work_func_t*, void* arg);

void thread_pool_wait(thread_pool_t* pool);
