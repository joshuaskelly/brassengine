/**
 * @file script.h
 * @brief Scripting module.
 */

#ifndef SCRIPT_H
#define SCRIPT_H

#include <stdbool.h>

enum b {
    SCRIPT_BUTTON_LEFT,
    SCRIPT_BUTTON_RIGHT,
    SCRIPT_BUTTON_UP,
    SCRIPT_BUTTON_DOWN,
    SCRIPT_BUTTON_A,
    SCRIPT_BUTTON_B,
};

/**
 * @brief Initialize scripting system. Called once during application start.
 */
void script_init(void);

/**
 * @brief Destroy scripting system. Called once during application shutdown.
 */
void script_destroy(void);

/**
 * @brief Update scripting system. Called at the beginning of an engine update
 * cycle.
 */
void script_update(void);

/**
 * @brief Draw script system. Called at the end of an engine update cycle.
 */
void script_draw(void);

/**
 * @brief Handle given event.
 *
 * @param event Event to handle.
 * @return true if event was handled, false otherwise.
 */
bool script_handle_event(event_t* event);

/**
 * @brief Invoke _init() global in Lua VM after all engine systems are ready.
 * @note Do we still need this?
 */
void script_setup(void);

/**
 * @brief Reload current script and reset VM state.
 */
void script_reload(void);

#endif
