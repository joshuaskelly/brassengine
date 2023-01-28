/**
 * @file console.h
 * System for in-game console.
 */

#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdbool.h>

#include "event.h"

/**
 * Initialize console system.
 */
void console_init(void);

/**
 * Destroy console system.
 */
void console_destroy(void);

/**
 * Handle given event.
 *
 * @param event Event to handle.
 * @return true if event was handled, false otherwise.
 */
bool console_handle_event(event_t* event);

/**
 * Update console system. Called at the beginning of an engine update
 * cycle.
 */
void console_update(void);

/**
 * Draw console system. Called at the end of an engine update cycle.
 */
void console_draw(void);

/**
 * Write line to console output history buffer.
 *
 * @param line
 */
void console_buffer_write(const char* line);

/**
 * Clear console output history buffer.
 */
void console_buffer_clear(void);

/**
 * Toggle visibilty of console in game.
 */
void console_buffer_toggle(void);

#endif
