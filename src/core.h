/**
 * @file core.h
 * @brief Core engine functionality.
 */
#ifndef CORE_H
#define CORE_H

#include "state.h"

/**
 * @brief Initialize all engine systems
 */
void core_init(void);

/**
 * @brief Destroy all engine systems
 */
void core_destroy(void);

/**
 * @brief Run engine
 */
void core_run(void);

/**
 * @brief Engine main loop
 */
void core_main_loop(void);

/**
 * @brief Set and initialize state
 *
 * @param state New state to set as current
 */
void core_set_state(state_t* state);

#endif