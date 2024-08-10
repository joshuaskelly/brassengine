/**
 * @file input.h
 * Input module. Manages state of input devices.
 */

#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

#include "event.h"

/**
 * Initialize input system. Called once during application start.
 */
void input_init(void);

/**
 * Destroy input system. Called once during application start.
 */
void input_destroy(void);

/**
 * Update system. Called at the beginning of an engine update cycle.
 */
void input_update(void);

/**
 * Handle given event.
 *
 * @param event Event to handle.
 * @return true if event was handled, false otherwise.
 */
void input_handle_event(event_t* event);

/**
 * Check if given key code is down.
 *
 * @param code Key code to check
 * @return true if down, false otherwise.
 */
bool input_keyboard_is_key_code_down(key_code_t code);

/**
 * Check if given key symbol is down.
 *
 * @param symbol Key symbol to check
 * @return true if down, false otherwise.
 */
bool input_keyboard_is_key_symbol_down(key_symbol_t symbol);

/**
 * Check if given mouse button is down.
 *
 * @param button Button to check
 * @return true if down, false otherwise.
 */
bool input_mouse_is_button_pressed(int button);

/**
 * Get mouse position
 *
 * @param x Mouse x position as reference
 * @param y Mouse y position as reference
 */
void input_mouse_position(int* x, int* y);

/**
 * Get mouse relative motion.
 *
 * @param x Mouse delta x as reference
 * @param y Mouse delta y as reference
 */
void input_mouse_motion(int* x, int* y);

/**
 * Get mouse wheel values.
 *
 * @param horizontal Mouse horizontal scroll amount as reference.
 * @param vertical Mouse vertical scroll amount as reference.
*/
void input_mouse_wheel(int* horizontal, int* vertical);

#endif
