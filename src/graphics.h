/**
 * @file graphics.h
 * Graphics module. Responsible for manipulating pixels.
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int x;
    int y;
    int width;
    int height;
} rect_t;

typedef uint8_t color_t;

typedef struct {
    int width;
    int height;
    int stride;
    bool is_subtexture;
    color_t* pixels;
} texture_t;

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
texture_t* graphics_render_texture_get(void);

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
color_t* graphics_draw_palette_get(void);

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
void graphics_pixel_set(int x, int y, color_t color);

/**
 * Function to copy a pixel from the source texture to the destination texture.
 */
typedef void(*pixel_copy_func_t)(
    texture_t* source_texture,
    texture_t* destination_texture,
    int sx,
    int sy,
    int dx,
    int dy
);

/**
 * Copy pixels from source texture to destination texture.
 *
 * @param source_texture Texture to copy from
 * @param destination_texture Texture to copy to. NULL to copy to the render texture
 * @param source_rect Rect representing area to copy from. NULL to copy from everything
 * @param destination_rect Rect representing area to copy to. NULL to copy to everything
 * @param func Function used to set pixels. NULL to use default pixel copy function
 */
void graphics_blit(
    texture_t* source_texture,
    texture_t* destination_texture,
    rect_t* source_rect,
    rect_t* destination_rect,
    pixel_copy_func_t func
);

/**
 * Sets render buffer resolution.
 *
 * @param width Width of render buffer in pixels.
 * @param height Height of render buffer in pixels.
 */
void graphics_resolution_set(int width, int height);

/**
 * Sets clipping rectangle which defines drawable area.
 *
 * @param x Rect top left x-coordinate
 * @param y Rect top left y-coordinate
 * @param width Rect width
 * @param height Rect height
 */
void graphics_clipping_rectangle_set(rect_t* rect);

/**
 * Gets clipping rectangle which defines the drawable area.
 *
 * @return rect_t Clipping rectangle
 */
rect_t* graphics_clipping_rectangle_get(void);

#endif
