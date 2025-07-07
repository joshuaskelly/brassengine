#ifndef THREADS_H
#define THREADS_H

#include <stdlib.h>

typedef struct thread thread_t;

/**
 * Create a new thread.
 *
 * @param function Thread main entry point function
 * @param args Single argument passed to entry point function
 * @return New thread if successful, NULL otherwise
 */
thread_t* threads_thread_new(void* (function)(void*), void* args);

/**
 * Frees a thread.
 *
 * @param thread Thread to free
 */
void threads_thread_free(thread_t* thread);

/**
 * Detatches a thread. Once detatched a thread can no longer be joined.
 *
 * @param thread Thread to detatch
 */
void threads_thread_detatch(thread_t* thread);

/**
 * Wait until given thread finishes execution.
 *
 * @param thread Thread to join
 * @return Result from thread upon finishing execution
 */
void* threads_thread_join(thread_t* thread);

/**
 * Terminate given thread and return result to join. Called from inside
 * exiting thread.
 *
 * @param result Result to make availble to threads_thread_join call
 */
void threads_thread_exit(void* result);

typedef struct thread_lock thread_lock_t;

/**
 * Creates a new thread lock.
 *
 * @return New thread lock if successful, NULL otherwise
 */
thread_lock_t* threads_thread_lock_new(void);

/**
 * Frees a thread lock.
 *
 * @param lock Thread lock to free
 */
void threads_thread_lock_free(thread_lock_t* lock);

/**
 * Locks given thread lock. Only one thread at a time may have access to a
 * lock. This function will wait until the lock can be acquired.
 *
 * @param lock Thread lock to lock
 */
void threads_lock_lock(thread_lock_t* lock);

/**
 * Unlocks given thread lock. This must be done to allow other threads to
 * continue their execution.
 *
 * @param lock Thread lock to unlock
 */
void threads_lock_unlock(thread_lock_t* lock);

typedef struct thread_condition thread_condition_t;

/**
 * Creates a new thread condition.
 *
 * @return New thread condition if successful, NULL otherwise
 */
thread_condition_t* threads_thread_condition_new(void);

/**
 * Frees a thread condition.
 *
 * @param condition Thread condition to free
 */
void threads_thread_condition_free(thread_condition_t* condition);

/**
 * Pause thread execution until given condition has been alerted. A thread lock
 * can be given to ensure only one thread will respond to the alert.
 *
 * @param condition Condition to wait for
 * @param lock Lock to lock once waiting is over
 */
void threads_thread_condition_wait(thread_condition_t* condition, thread_lock_t* lock);

/**
 * Alert threads waiting on given condition.
 *
 * @param condition Condition to alert
 */
void threads_thread_condition_alert(thread_condition_t* condition);

typedef struct thread_pool thread_pool_t;

/**
 * Create a new thead pool.
 *
 * @param count Number of worker threads
 * @return New thread pool if successful, NULL otherwise
 */
thread_pool_t* threads_thread_pool_new(size_t count);

/**
 * Frees a thread pool. Will wait for all threads to exit before cleaning up.
 *
 * @param thread_pool Thread pool to free
 */
void threads_thread_pool_free(thread_pool_t* thread_pool);

/**
 * Add work to the thread pool.
 *
 * @param thread_pool Thread pool to add work to
 * @param function Thread main entry point function
 * @param args Single argument passed to entry point function
 */
void threads_thread_pool_add_work(thread_pool_t* thread_pool, void(function)(void*), void* arg);

/**
 * Wait for all work in thread pool to complete.
 *
 * @param thread_pool Thread pool to wait on
 */
void threads_thread_pool_wait(thread_pool_t* thread_pool);

#endif
