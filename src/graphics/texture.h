#ifndef GRAPHICS_TEXTURE_H
#define GRAPHICS_TEXTURE_H

#include <stdbool.h>
#include <stdint.h>

#include "../graphics/types.h"

/**
 * Create a new texture.
 *
 * @param width Width of texture in pixels
 * @param height Height of texture in pixels
 * @param pixels Pixel data to copy or NULL
 * @return New texture if successful, NULL otherwise
 */
texture_t* graphics_texture_new(int width, int height, const color_t* pixels);

/**
 * Frees a texture.
 *
 * @param texture Texture to free
 */
void graphics_texture_free(texture_t* texture);

/**
 * Get size of texture struct including size of pixel data
 *
 * @param texture Texture to get size of
 * @return Size of given texture
 */
size_t graphics_texture_sizeof(texture_t* texture);

/**
 * Get pointer to texture pixels.
 *
 * @param texture Texture to get pixels for
 * @return Pointer to texture pixels
 */
color_t* graphics_texture_pixels_get(texture_t* texture);

/**
 * Get texture width in pixels.
 *
 * @param texture Texture to get width for
 * @return Width of given texture in pixels
 */
int graphics_texture_width_get(texture_t* texture);

/**
 * Get texture height in pixels.
 *
 * @param texture Texture to get height for
 * @return Height of given texture in pixels
 */
int graphics_texture_height_get(texture_t* texture);

/**
 * Copy given texture
 *
 * @param texture Texture to copy
 * @return texture_t* New texture if successful, NULL otherwise
 */
texture_t* graphics_texture_copy(texture_t* texture);

/**
 * Fill entire texture with color.
 *
 * @param texture Texture to fill
 * @param color Fill color
 */
void graphics_texture_clear(texture_t* texture, color_t color);

/**
 * Creates a texture that shares pixels with the given texture.
 *
 * @param texture Texture to get subtexture from
 * @param rect Sub region of given texture to use as subtexture.
 * @return New texture if successful, NULL otherwise.
 */
texture_t* graphics_texture_sub(texture_t* texture, rect_t* rect);

/**
 * Set pixel color.
 *
 * @param texture Texture to set pixel
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @param color Pixel color
 */
void graphics_texture_pixel_set(texture_t* texture, int x, int y, color_t color);

/**
 * Get pixel color.
 *
 * @param texture Texture to get pixel
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @return Color at given coordinates
 */
color_t graphics_texture_pixel_get(texture_t* texture, int x, int y);

/**
 * Copy a portion of one texture to another.
 *
 * @param source_texture Texture to copy from
 * @param destination_texture Texture to copy to
 * @param source_rect Sub region of source texture. NULL for entire source texture
 * @param destination_rect Destination region. NULL for entire destination texture, texture will be stretched.
 */
void graphics_texture_blit(texture_t* source_texture, texture_t* destination_texture, rect_t* source_rect, rect_t* destination_rect);

#endif
