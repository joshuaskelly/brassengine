/**
 * @file platform.h
 * Platform specific interface. A platform implementation must implement
 * all these methods.
 *
 * @see src/platforms/desktop.c
 * @see src/platforms/web.c
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdbool.h>

#include "event.h"
#include "sounds.h"
#include "threads.h"

/**
 * Platform specific engine main entry point.
 *
 * @param argc Count of command line arguments.
 * @param argv Vector of C strings passed in from command line.
 */
int platform_main(int argc, char* argv[]);

/**
 * Initialize system. Called once during application start.
 */
void platform_init(void);

/**
 * Destroy system. Called once during application shutdown.
 */
void platform_destroy(void);

/**
 * Reload system.
 */
void platform_reload(void);

/**
 * Update system. Called at the beginning of an engine update cycle.
 */
void platform_update(void);

/**
 * Draw system. Called at the end of an engine update cycle.
 */
void platform_draw(void);

/**
 * Handle given event.
 */
bool platform_handle_event(event_t* event);

/**
 * Plays given sound.
 */
void platform_sound_play(sound_t* sound, int channel, bool looping);

/**
 * Stops playback for given channel. If channel is -1 all channels will be
 * stopped.
 */
void platform_sound_stop(int channel);

/**
 * Sets volume for given channel.
 */
void platform_sound_volume(int channel, float volume);

/**
 * Set mouse grab state. If true, mouse will be constrained to window.
 *
 * @param grabbed
 */
void platform_mouse_grabbed_set(bool grabbed);

/**
 * Get mouse grab state.
 *
 * @return bool Is mouse grabbed?
 */
bool platform_mouse_grabbed_get(void);

/**
 * Open platform specific module.
 *
 * @param arg Scipting engine specific context data. Typically this is a
 * lua_State pointer.
 */
void platform_open_module(void* arg);

/**
 * Create a new thread.
 *
 * @param function Thread main entry point function
 * @param args Single argument passed to entry point function
 * @return New thread if successful, NULL otherwise
 */
thread_t* platform_thread_new(void* (function)(void*), void* args);

/**
 * Frees a thread.
 *
 * @param thread Thread to free
 */
void platform_thread_free(thread_t* thread);

/**
 * Detatches a thread. Once detatched a thread can no longer be joined.
 *
 * @param thread Thread to detatch
 */
void platform_thread_detatch(thread_t* thread);

/**
 * Wait until given thread finishes execution.
 *
 * @param thread Thread to join
 * @return Result from thread upon finishing execution
 */
void* platform_thread_join(thread_t* thread);

/**
 * Terminate given thread and return result to join. Called from inside
 * exiting thread.
 *
 * @param result Result to make availble to threads_thread_join call
 */
void platform_thread_exit(void* result);

/**
 * Creates a new thread lock.
 *
 * @return New thread lock if successful, NULL otherwise
 */
thread_lock_t* platform_thread_lock_new(void);

/**
 * Frees a thread lock.
 *
 * @param lock Thread lock to free
 */
void platform_thread_lock_free(thread_lock_t* lock);

/**
 * Locks given thread lock. Only one thread at a time may have access to a
 * lock. This function will wait until the lock can be acquired.
 *
 * @param lock Thread lock to lock
 */
void platform_thread_lock_lock(thread_lock_t* lock);

/**
 * Unlocks given thread lock. This must be done to allow other threads to
 * continue their execution.
 *
 * @param lock Thread lock to unlock
 */
void platform_thread_lock_unlock(thread_lock_t* lock);

/**
 * Creates a new thread condition.
 *
 * @return New thread condition if successful, NULL otherwise
 */
thread_condition_t* platform_thread_condition_new(void);

/**
 * Frees a thread condition.
 *
 * @param condition Thread condition to free
 */
void platform_thread_condition_free(thread_condition_t* condition);

/**
 * Pause thread execution until given condition has been alerted. A thread lock
 * can be given to ensure only one thread will respond to the alert.
 *
 * @param condition Condition to wait for
 * @param lock Lock to lock once waiting is over
 */
void platform_thread_condition_wait(thread_condition_t* condition, thread_lock_t* lock);

/**
 * Notify threads waiting on given condition.
 *
 * @param condition Condition to notify threads about
 */
void platform_thread_condition_notify(thread_condition_t* condition);

#endif
