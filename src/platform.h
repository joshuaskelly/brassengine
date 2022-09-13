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
 * Update system. Called at the beginning of an engine update cycle.
 */
void platform_update(void);

/**
 * Draw system. Called at the end of an engine update cycle.
 */
void platform_draw(void);

#endif