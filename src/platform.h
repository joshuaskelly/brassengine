/**
 * @file platform.h
 * @brief Platform specific implementation.
 */

#ifndef PLATFORM_H
#define PLATFORM_H

/**
 * @brief Platform specific engine main entry point.
 *
 * @param argc Count of command line arguments.
 * @param argv Vector of C strings passed in from command line.
 */
int platform_main(int argc, char* argv[]);

/**
 * @brief Initialize backend system.
 */
void platform_init(void);

/**
 * @brief Destroy backend system.
 */
void platform_destroy(void);

/**
 * @brief Update system. Called at the beginning of an engine
 * update cycle.
 */
void platform_update(void);

/**
 * @brief Draw system. Called at the end of an engine update
 * cycle.
 */
void platform_draw(void);

#endif