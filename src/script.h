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
 * Complete the given expression
 *
 * @param expression String representing a Lua expression. It may be complete
 * or a fragment.
 * @return A more complete expression if possible. The original expression
 * otherwise.
 */
void script_complete(char* expression);

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
