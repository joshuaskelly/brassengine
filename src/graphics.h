#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

#define RENDER_BUFFER_WIDTH 320
#define RENDER_BUFFER_HEIGHT 200

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
 * @param height Heigh of texture in pixels
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
 * @param texture The texture to fill
 * @param color The fill color
 */
void texture_clear(texture_t* texture, color_t color);

/**
 * @brief Set pixel color.
 *
 * @param texture The texture to set pixel
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @param color Color to set pixel
 */
void texture_set_pixel(texture_t* texture, int x, int y, color_t color);

/**
 * @brief Get pixel color.
 *
 * @param texture The texture to get pixel
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @return Color at given coordinates
 */
color_t texture_get_pixel(texture_t* texture, int x, int y);

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

#endif
