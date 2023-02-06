/**
 * @file script.h
 * Scripting module.
 */

#ifndef SCRIPT_H
#define SCRIPT_H

#include <stdbool.h>

#include "event.h"

/**
 * Initialize scripting system. Called once during application start.
 */
void script_init(void);

/**
 * Destroy scripting system. Called once during application shutdown.
 */
void script_destroy(void);

/**
 * Update scripting system. Called at the beginning of an engine update
 * cycle.
 */
void script_update(void);

/**
 * Draw script system. Called at the end of an engine update cycle.
 */
void script_draw(void);

/**
 * Handle given event.
 *
 * @param event Event to handle.
 * @return true if event was handled, false otherwise.
 */
bool script_handle_event(event_t* event);

/**
 * Invoke _init() global in Lua VM after all engine systems are ready.
 * @note Do we still need this?
 */
void script_setup(void);

/**
 * Reload current script and reset VM state.
 */
void script_reload(void);

/**
 * Evaluate given script
 *
 * @param script String representing a Lua expression, statement, or sequence
 * of statements.
 * @return 0 if successful, error code otherwise.
 */
int script_evaluate(const char* script);

/**
 * Gets time used by _update function call in milliseconds
 *
 * @return Time in milliseconds
 */
double script_update_time_get(void);

/**
 * Gets time used by _draw function call in milliseconds
 *
 * @return Time in milliseconds
 */
double script_draw_time_get(void);

#endif
