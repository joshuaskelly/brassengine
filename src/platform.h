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
void platform_play_sound(sound_t* sound);

void platform_display_set_resolution(int width, int height);

/**
 * Set display size.
 *
 * @param width Display width.
 * @param height Display height.
 */
void platform_display_set_size(int width, int height);

/**
 * Set display fullscreen.
 *
 * @param fullscreen True to set to fullscreen, False for windowed.
 */
void platform_display_set_fullscreen(bool fullscreen);

/**
 * Set display title.
 *
 * @param title Text to set as title.
 */
void platform_display_set_title(const char* title);

void platform_mouse_set_grabbed(bool grabbed);

bool platform_mouse_get_grabbed(void);

#endif
