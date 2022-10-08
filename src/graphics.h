/**
 * @file graphics.h
 * Graphics module. Responsible for manipulating pixels.
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

#define RENDER_BUFFER_WIDTH 320
#define RENDER_BUFFER_HEIGHT 200

typedef struct {
    int x;
    int y;
    int width;
    int height;
} rect_t;

typedef uint8_t color_t;

typedef struct {
    color_t* pixels;
    int width;
    int height;
} texture_t;

/**
 * Create a new texture.
 *
 * @param width Width of texture in pixels
 * @param height Height of texture in pixels
 * @param pixels Pixel data to copy or NULL
 * @return New texture if successful, NULL otherwise
 */
texture_t* graphics_texture_new(int width, int height, color_t* pixels);

/**
 * Frees a texture.
 *
 * @param texture Texture to free
 */
void graphics_texture_free(texture_t* texture);

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
 * Set pixel color.
 *
 * @param texture Texture to set pixel
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @param color Pixel color
 */
void graphics_texture_set_pixel(texture_t* texture, int x, int y, color_t color);

/**
 * Get pixel color.
 *
 * @param texture Texture to get pixel
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @return Color at given coordinates
 */
color_t graphics_texture_get_pixel(texture_t* texture, int x, int y);

/**
 * Copy a portion of one texture to another.
 *
 * @param source_texture Texture to copy from
 * @param destination_texture Texture to copy to
 * @param source_rect Sub region of source texture. NULL for entire source texture
 * @param destination_rect Destination region. NULL for entire destination texture, texture will be stretched.
 */
void graphics_texture_blit(texture_t* source_texture, texture_t* destination_texture, rect_t* source_rect, rect_t* destination_rect);

/**
 * Initialize graphics system.
 */
void graphics_init(void);

/**
 * Destroy graphics system.
 */
void graphics_destroy(void);

/**
 * Gets render texture.
 *
 * @return Render texture.
 */
texture_t* graphics_get_render_texture(void);

/**
 * Get palette.
 *
 * @return Palette as a 256 color array.
 */
uint32_t* graphics_palette_get(void);

/**
 * Set palette.
 * @param palette 256 color array.
 */
void graphics_palette_set(uint32_t* palette);

/**
 * Reset all palette values.
 */
void graphics_palette_clear(void);

/**
 * Get draw palette.
 *
 * @return Palette as a 256 color array.
 */
uint32_t* graphics_draw_palette_get(void);

/**
 * Set draw palette.
 *
 * @param palette 256 color array.
 */
void graphics_draw_palette_set(uint32_t* palette);

/**
 * Reset all draw palette values.
 */
void graphics_draw_palette_clear(void);

/**
 * Set transparent color.
 *
 * @param color Color to set as transparent. A value of -1 is no transparency.
 */
void graphics_transparent_color_set(int color);

/**
 * Get transparent color.
 *
 * @return int Transparent color. A value of -1 is no transparency.
 */
int graphics_transparent_color_get(void);

/**
 * Set pixel color.
 *
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @param color Pixel color
 */
void graphics_set_pixel(int x, int y, color_t color);

void graphics_blit(texture_t* texture, rect_t* source_rect, rect_t* destination_rect);

/**
 * Sets clipping rectangle which defines drawable area.
 *
 * @param x Rect top left x-coordinate
 * @param y Rect top left y-coordinate
 * @param width Rect width
 * @param height Rect height
 */
void graphics_set_clipping_rectangle(int x, int y, int width, int height);

#endif
