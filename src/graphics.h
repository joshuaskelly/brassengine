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
 * @brief Create a new texture.
 *
 * @param width Width of texture in pixels
 * @param height Height of texture in pixels
 * @param pixels Pixel data to copy or NULL
 * @return New texture if successful, NULL on failure
 */
texture_t* texture_new(int width, int height, color_t* pixels);

/**
 * @brief Frees a texture.
 *
 * @param texture Texture to free
 */
void texture_free(texture_t* texture);

/**
 * @brief Fill entire texture with color.
 *
 * @param texture Texture to fill
 * @param color Fill color
 */
void texture_clear(texture_t* texture, color_t color);

/**
 * @brief Set pixel color.
 *
 * @param texture Texture to set pixel
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @param color Pixel color
 */
void texture_set_pixel(texture_t* texture, int x, int y, color_t color);

/**
 * @brief Get pixel color.
 *
 * @param texture Texture to get pixel
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @return Color at given coordinates
 */
color_t texture_get_pixel(texture_t* texture, int x, int y);

void texture_blit(texture_t* source, texture_t* destination, rect_t* source_rect, rect_t* destination_rect);

/**
 * @brief Initialize graphics system.
 */
void graphics_init(void);

/**
 * @brief Destroy graphics system.
 */
void graphics_destroy(void);

/**
 * @brief Gets render texture.
 *
 * @return Render texture.
 */
texture_t* graphics_get_render_texture(void);

/**
 * @brief Get palette.
 *
 * @return Palette as a 256 color array.
 */
uint32_t* graphics_get_palette(void);

/**
 * @brief Set pixel color.
 *
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @param color Pixel color
 */
void graphics_set_pixel(int x, int y, color_t color);

/**
 * @brief Sets clipping rectangle which defines drawable area.
 *
 * @param x Rect top left x-coordinate
 * @param y Rect top left y-coordinate
 * @param width Rect width
 * @param height Rect height
 */
void graphics_set_clipping_rectangle(int x, int y, int width, int height);

#endif
