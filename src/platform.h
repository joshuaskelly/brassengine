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

#include <stdbool.h>

#include "sounds.h"

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
 * Reload system.
 */
void platform_reload(void);

/**
 * Update system. Called at the beginning of an engine update cycle.
 */
void platform_update(void);

/**
 * Draw system. Called at the end of an engine update cycle.
 */
void platform_draw(void);

/**
 * Plays given sound.
 */
void platform_sound_play(sound_t* sound, int channel);

void platform_display_resolution_set(int width, int height);

/**
 * Set display size.
 *
 * @param width Display width.
 * @param height Display height.
 */
void platform_display_size_set(int width, int height);

/**
 * Set display fullscreen.
 *
 * @param fullscreen True to set to fullscreen, False for windowed.
 */
void platform_display_fullscreen_set(bool fullscreen);

/**
 * Set display title.
 *
 * @param title Text to set as title.
 */
void platform_display_title_set(const char* title);

/**
 * Set mouse grab state. If true, mouse will be constrained to window.
 *
 * @param grabbed
 */
void platform_mouse_grabbed_set(bool grabbed);

/**
 * Get mouse grab state.
 *
 * @return bool Is mouse grabbed?
 */
bool platform_mouse_grabbed_get(void);

#endif
