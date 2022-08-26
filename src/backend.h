/**
 * @file backend.h
 * @brief Platform specific implementation.
 */

#ifndef BACKEND_H
#define BACKEND_H

/**
 * @brief Platform specific engine main entry point.
 *
 * @param argc Count of command line arguments.
 * @param argv Vector of C strings passed in from command line.
 */
int backend_main(int argc, char* argv[]);

/**
 * @brief Initialize backend system.
 */
void backend_init(void);

/**
 * @brief Destroy backend system.
 */
void backend_destroy(void);

/**
 * @brief Update system. Called at the beginning of an engine
 * update cycle.
 */
void backend_update(void);

/**
 * @brief Draw system. Called at the end of an engine update
 * cycle.
 */
void backend_draw(void);

#endif