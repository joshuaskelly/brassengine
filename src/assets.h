/**
 * @file assets.h
 * Asset module. Responsible for managing assets. Knows how to process
 * various file formats into engine usable forms.
 */

#ifndef ASSETS_H
#define ASSETS_H

#include <stdbool.h>
#include <stdint.h>

#include "graphics.h"

/**
 * Initialize assets system.
 */
void assets_init(void);

/**
 * Destroy assets system.
 */
void assets_destroy(void);

/**
 * Load all assets.
 *
 * @return true if successful, false otherwise
 */
bool assets_load(void);

/**
 * Unload all assets.
 */
void assets_unload(void);

/**
 * Reload all assets.
 */
void assets_reload(void);

/**
 * Get texture for given filename.
 *
 * @param filename Name to search for.
 * @return texture_t* texture if found, NULL otherwise
 */
texture_t* assets_get_texture(const char* filename);

/**
 * Get script for given filename.
 *
 * @param filename Name to search for.
 * @return const char* script text if found, NULL otherwise
 */
const char* assets_get_script(const char* filename);

#endif
