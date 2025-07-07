#include <stdbool.h>
#include <stdlib.h>

#include "platform.h"
#include "threads.h"

thread_t* threads_thread_new(void* (function)(void*), void* args) {
    return platform_thread_new(function, args);
}

void threads_thread_free(thread_t* thread) {
    platform_thread_free(thread);
}

void threads_thread_detatch(thread_t* thread) {
    platform_thread_detatch(thread);
}

void* threads_thread_join(thread_t* thread) {
    return platform_thread_join(thread);
}

void threads_thread_exit(void* result) {
    platform_thread_exit(result);
}
thread_lock_t* threads_thread_lock_new(void) {
    return platform_thread_lock_new();
}

void threads_thread_lock_free(thread_lock_t* lock) {
    platform_thread_lock_free(lock);
}

void threads_lock_lock(thread_lock_t* lock) {
    platform_thread_lock_lock(lock);
}

void threads_lock_unlock(thread_lock_t* lock) {
    platform_thread_lock_unlock(lock);
}

thread_condition_t* threads_thread_condition_new(void) {
    return platform_thread_condition_new();
}

void threads_thread_condition_free(thread_condition_t* condition) {
    platform_thread_condition_free(condition);
}

void threads_thread_condition_wait(thread_condition_t* condition, thread_lock_t* lock) {
    platform_thread_condition_wait(condition, lock);
}

void threads_thread_condition_alert(thread_condition_t* condition) {
    platform_thread_condition_alert(condition);
}

typedef void(thread_pool_work_function_t)(void*);

/**
 * Internal structure used to represent a unit of work.
 */
typedef struct thread_pool_work {
    thread_pool_work_function_t* function;
    void* arg;
    struct thread_pool_work* next;

} thread_pool_work_t;

/**
 * Create a new thread pool work object.
 *
 * @return New thread pool work object if successful, NULL otherwise
 */
static thread_pool_work_t* thread_pool_work_new(thread_pool_work_function_t function, void* arg) {
    thread_pool_work_t* work = NULL;

    if (function == NULL) {
        return NULL;
    }

    work = (thread_pool_work_t*)malloc(sizeof(thread_pool_work_t));

    work->function = function;
    work->arg = arg;
    work->next = NULL;

    return work;
}

/**
 * Frees a thread pool work object.
 *
 * @param work Thread pool work object to free
 */
static void thread_pool_work_free(thread_pool_work_t* work) {
    if (work == NULL) return;

    free(work);
    work = NULL;
}

struct thread_pool {
    thread_pool_work_t* head;
    thread_pool_work_t* tail;
    thread_lock_t* lock;
    thread_condition_t* work_available;
    thread_condition_t* work_finished;
    size_t thread_count;
    size_t active_thread_count;
    bool stop;
};

static thread_pool_work_t* work_get(thread_pool_t* pool);
static void* worker_thread_main(void* arg);

thread_pool_t* threads_thread_pool_new(size_t count) {
    thread_pool_t* pool;

    if (count <= 0) {
        count = 4;
    }

    pool = (thread_pool_t*)malloc(sizeof(thread_pool_t));

    pool->head = NULL;
    pool->tail = NULL;
    pool->lock = threads_thread_lock_new();
    pool->work_available = threads_thread_condition_new();
    pool->work_finished = threads_thread_condition_new();
    pool->thread_count = count;
    pool->active_thread_count = 0;

    thread_t* thread = NULL;
    for (size_t i = 0; i < count; i++) {
        thread = threads_thread_new(worker_thread_main, pool);
        threads_thread_detatch(thread);
        threads_thread_free(thread);
    }

    return pool;
}

void threads_thread_pool_add_work(thread_pool_t* thread_pool, void(function)(void*), void* arg) {
    if (!thread_pool) return;

    thread_pool_work_t* work = thread_pool_work_new(function, arg);
    if (!work) return;

    threads_lock_lock(thread_pool->lock);

    // Append work to queue
    if (thread_pool->head == NULL) {
        thread_pool->head = work;
    }
    else {
        thread_pool->tail->next = work;
    }

    thread_pool->tail = work;

    threads_thread_condition_alert(thread_pool->work_available);
    threads_lock_unlock(thread_pool->lock);
}

void threads_thread_pool_wait(thread_pool_t* thread_pool) {
    if (!thread_pool) return;

    threads_lock_lock(thread_pool->lock);

    while (true) {
        bool work_pending = thread_pool->head != NULL;
        bool doing_work = !thread_pool->stop && thread_pool->active_thread_count != 0;
        bool shutting_down = thread_pool->stop && thread_pool->thread_count != 0;

        if (work_pending || doing_work || shutting_down) {
            threads_thread_condition_wait(thread_pool->work_finished, thread_pool->lock);
        }
        else
        {
            break;
        }
    }

    threads_lock_unlock(thread_pool->lock);
}

void threads_thread_pool_free(thread_pool_t* thread_pool) {
    if (thread_pool == NULL) return;

    thread_pool_work_t* current = NULL;
    thread_pool_work_t* next = NULL;

    // Clear out work queue
    current = thread_pool->head;
    while(current != NULL) {
        next = current->next;
        thread_pool_work_free(current);
        current = next;
    }

    thread_pool->head = NULL;
    thread_pool->tail = NULL;
    thread_pool->stop = true;

    // Alert threads to exit
    threads_thread_condition_alert(thread_pool->work_available);
    threads_lock_unlock(thread_pool->lock);

    // Wait for threads to exit
    threads_thread_pool_wait(thread_pool);

    threads_thread_lock_free(thread_pool->lock);
    threads_thread_condition_free(thread_pool->work_available);
    threads_thread_condition_free(thread_pool->work_finished);

    free(thread_pool);
    thread_pool = NULL;
}

/**
 * Get first available work object.
 *
 * @return Thread work object if any work available, NULL otherwise
 */
static thread_pool_work_t* work_get(thread_pool_t* pool) {
    thread_pool_work_t* work = NULL;

    if (pool == NULL) {
        return NULL;
    }

    work = pool->head;
    if (work == NULL) {
        return NULL;
    }

    if (work->next == NULL) {
        pool->head = NULL;
        pool->tail = NULL;
    }
    else {
        pool->head = work->next;
    }

    return work;
}

/**
 * Main worker thread processing loop. Will attempt to get work from the pool
 * to work on, otherwise will wait until work becomes available.
 *
 * @param arg Thread pool that owns the thread
 * @return Always NULL
 */
static void* worker_thread_main(void* arg) {
    thread_pool_t* pool = (thread_pool_t*)arg;
    thread_pool_work_t* work = NULL;

    while (true) {
        threads_lock_lock(pool->lock);

        // Wait for work to become available
        while (pool->head == NULL && !pool->stop) {
            threads_thread_condition_wait(pool->work_available, pool->lock);
        }

        // Shutting down, exit loop
        if (pool->stop) break;

        // Get next available work
        work = work_get(pool);

        // Update active thread count
        pool->active_thread_count++;

        threads_lock_unlock(pool->lock);

        // Do the work
        if (work != NULL) {
            work->function(work->arg);
            thread_pool_work_free(work);
        }

        threads_lock_lock(pool->lock);

        // Update active thread count
        pool->active_thread_count--;

        // Alert if all available work is finished
        if (!pool->stop && pool->active_thread_count == 0 && pool->head == NULL) {
            threads_thread_condition_alert(pool->work_finished);
        }

        threads_lock_unlock(pool->lock);
    }

    // Update total thread count
    pool->thread_count--;

    // Alert other threads to clean themselves up
    threads_thread_condition_alert(pool->work_available);

    // Alert pool that all threads are cleaned up
    if (pool->thread_count == 0) {
        threads_thread_condition_alert(pool->work_finished);
    }

    threads_lock_unlock(pool->lock);

    return NULL;
}
