/**
 * @file assets.h
 * Asset module. Responsible for managing assets. Knows how to process
 * various file formats into engine usable forms.
 */

#ifndef ASSETS_H
#define ASSETS_H

#include <stdbool.h>
#include <stdio.h>

#include "graphics.h"
#include "sounds.h"

/**
 * Initialize assets system.
 */
void assets_init(void);

/**
 * Destroy assets system.
 */
void assets_destroy(void);

/**
 * Reload all assets.
 */
void assets_reload(void);

/**
 * Opens a file from inside asset directory or zip file.
 *
 * @param filename Name of file to open
 * @param mode File access mode
 * @return FILE* File stream pointer if successful, NULL otherwise.
 */
FILE* assets_open_file(const char* filename, const char* mode);

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

/**
 * Get sound for given filename.
 *
 * @param filename Name to search for.
 * @return sound_t* sound if found, NULL otherwise
 */
sound_t* assets_get_sound(const char* filename);

/**
 * Get shader for given filename.
 *
 * @param filename Name to search for.
 * @return const char* shader if found, NULL otherwise
 */
const char* assets_get_shader(const char* filename);

/**
 * Save sequence of textures as an animated GIF.
 *
 * @param filename Name of file to save.
 * @param frame_count Frame count.
 * @param frames Array of frames.
 */
void assets_gif_save(const char* filename, int frame_count, texture_t** frames);

extern char* assets_directory;

extern const uint8_t default_font_pixels[16384];
extern const char* default_shader;

#endif
