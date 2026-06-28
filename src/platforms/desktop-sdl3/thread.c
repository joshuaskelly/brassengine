/**
 * Pthread platform thread implementation
 */

#include <pthread.h>

#include "../../threads.h"

struct thread {
    pthread_t pthread;
};

thread_t* platform_thread_new(void* (function)(void*), void* args) {
    thread_t* thread = (thread_t*)malloc(sizeof(thread_t));
    pthread_create(&thread->pthread, NULL, function, args);

    return thread;
}

void platform_thread_free(thread_t* thread) {
    if (thread == NULL) return;

    free(thread);
}

void platform_thread_detatch(thread_t* thread) {
    pthread_detach(thread->pthread);
}

void* platform_thread_join(thread_t* thread) {
    void* result = NULL;
    pthread_join(thread->pthread, result);
    free(thread);

    return result;
}

void platform_thread_exit(void* result) {
    pthread_exit(result);
}

struct thread_lock {
    pthread_mutex_t mutex;
};

thread_lock_t* platform_thread_lock_new(void) {
    thread_lock_t* lock = NULL;
    lock = (thread_lock_t*)malloc(sizeof(thread_lock_t));

    pthread_mutex_init(&lock->mutex, NULL);

    return lock;
}

void platform_thread_lock_free(thread_lock_t* lock) {
    free(lock);
}

void platform_thread_lock_lock(thread_lock_t* lock) {
    pthread_mutex_lock(&(lock->mutex));
}

void platform_thread_lock_unlock(thread_lock_t* lock) {
    pthread_mutex_unlock(&(lock->mutex));
}

struct thread_condition {
    pthread_cond_t cond;
};

thread_condition_t* platform_thread_condition_new(void) {
    thread_condition_t* condition = (thread_condition_t*)malloc(sizeof(thread_condition_t));

    pthread_cond_init(&(condition->cond), NULL);

    return condition;
}

void platform_thread_condition_free(thread_condition_t* condition) {
    if (condition == NULL) return;

    pthread_cond_destroy(&(condition->cond));
    free(condition);
}

void platform_thread_condition_wait(thread_condition_t* condition, thread_lock_t* lock) {
    pthread_cond_wait(&(condition->cond), &(lock->mutex));
}

void platform_thread_condition_notify(thread_condition_t* condition) {
    pthread_cond_signal(&(condition->cond));
}
