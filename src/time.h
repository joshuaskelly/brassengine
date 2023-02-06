/**
 * @file time.h
 * Time module.
 */

#ifndef TIME_H
#define TIME_H

/**
 * Initialize time system. Called once during application start.
 */
void time_init(void);

/**
 * Destroy time system. Called once during application shutdown.
 */
void time_destroy(void);

/**
 * Update time system. Called at the end of an engine update
 * cycle.
 */
void time_update(void);

/**
 * Reset time state.
 */
void time_reload(void);

/**
 * Time interval since last frame in milliseconds.
 *
 * @return double Interval time in milliseconds.
 */
double time_delta_time(void);

/**
 * Time interval since engine launch in milliseconds.
 *
 * @return double Interval time in milliseconds.
 */
double time_since_init(void);

double time_millis_get(void);

#endif
