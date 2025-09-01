#ifndef GRAPHICS_DRAW_H
#define GRAPHICS_DRAW_H

#include "types.h"

/**
 * Draw pixel at x, y with given color
 *
 * @param destination Texture to draw to
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @param color Pixel color
 */
void graphics_draw_pixel(texture_t* destination, int x, int y, color_t color);

/**
 * Draw line from x0, y0 to x1, y1.
 *
 * @param destination Texture to draw to
 * @param x0 Start x-coordinate
 * @param y0 Start y-coordinate
 * @param x1 End x-coordinate
 * @param y1 End y-coordinate
 * @param color Line color
 */
void graphics_draw_line(texture_t* destination, int x0, int y0, int x1, int y1, color_t color);

/**
 * Draw line from x0, y0 to x1, y1 with given pattern.
 *
 * @param destination Texture to draw to
 * @param x0 Start x-coordinate
 * @param y0 Start y-coordinate
 * @param x1 End x-coordinate
 * @param y1 End y-coordinate
 * @param pattern Texture to use as a pattern
 * @param offset_x Pattern x-axis offset
 * @param offset_y Pattern y-axis offset
 */
void graphics_draw_pattern_line(texture_t* destination, int x0, int y0, int x1, int y1, texture_t* pattern, int pattern_offset_x, int pattern_offset_y);

/**
 * Draw line using affine texture mapping.
 *
 * @param destination Texture to draw to
 * @param x0 Start x-coordinate
 * @param y0 Start y-coordinate
 * @param u0 Start 0 u-coordinate
 * @param v0 Start 0 v-coordinate
 * @param x1 End x-coordinate
 * @param y1 End y-coordinate
 * @param u1 End u-coordinate
 * @param v1 End v-coordinate
 * @param texture_map Texture to map
 */
void graphics_draw_textured_line(texture_t* destination, int x0, int y0, float u0, float v0, int x1, int y1, float u1, float v1, texture_t* texture_map);

/**
 * Draw bezier curve
 *
 * @param destination Texture to draw to
 * @param x0 Start anchor point x-coordinate
 * @param y0 Start anchor point y-coordinate
 * @param x1 Start control point x-coordinate
 * @param y1 Start control point y-coordinate
 * @param x2 End control point x-coordinate
 * @param y2 End control point y-coordinate
 * @param x3 End anchor point x-coordinate
 * @param y3 End anchor point y-coordinate
 * @param color Cuve color
 */
void graphics_draw_bezier(texture_t* destination, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, color_t color);

/**
 * Draw bezier curve
 *
 * @param destination Texture to draw to
 * @param x0 Start anchor point x-coordinate
 * @param y0 Start anchor point y-coordinate
 * @param x1 Start control point x-coordinate
 * @param y1 Start control point y-coordinate
 * @param x2 End control point x-coordinate
 * @param y2 End control point y-coordinate
 * @param x3 End anchor point x-coordinate
 * @param y3 End anchor point y-coordinate
 * @param pattern Texture to use as a pattern
 * @param offset_x Pattern x-axis offset
 * @param offset_y Pattern y-axis offset
 */
void graphics_draw_pattern_bezier(texture_t* destination, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, texture_t* pattern, int pattern_offset_x, int pattern_offset_y);

/**
 * Draw rectangle.
 *
 * @param destination Texture to draw to
 * @param x Rect top left x-coordinate
 * @param y Rect top left y-coordinate
 * @param width Rect width
 * @param height Rect height
 * @param color Line color
 */
void graphics_draw_rectangle(texture_t* destination, int x, int y, int width, int height, color_t color);

/**
 * Draw rectangle with given pattern.
 *
 * @param destination Texture to draw to
 * @param x Rect top left x-coordinate
 * @param y Rect top left y-coordinate
 * @param width Rect width
 * @param height Rect height
 * @param pattern Texture to use as a pattern
 * @param offset_x Pattern x-axis offset
 * @param offset_y Pattern y-axis offset
 */
void graphics_draw_pattern_rectangle(texture_t* destination, int x, int y, int width, int height, texture_t* pattern, int pattern_offset_x, int pattern_offset_y);

/**
 * Draw filled rectangle.
 *
 * @param destination Texture to draw to
 * @param x Rect top left x-coordinate
 * @param y Rect top left y-coordinate
 * @param width Rect width
 * @param height Rect height
 * @param color Fill color
 */
void graphics_draw_filled_rectangle(texture_t* destination, int x, int y, int width, int height, color_t color);

/**
 * Draw filled rectangle with given pattern.
 *
 * @param destination Texture to draw to
 * @param x Rect top left x-coordinate
 * @param y Rect top left y-coordinate
 * @param width Rect width
 * @param height Rect height
 * @param pattern Texture to use as a pattern
 * @param offset_x Pattern x-axis offset
 * @param offset_y Pattern y-axis offset
 */
void graphics_draw_filled_pattern_rectangle(texture_t* destination, int x, int y, int width, int height, texture_t* pattern, int pattern_offset_x, int pattern_offset_y);

/**
 * Draw circle.
 *
 * @param destination Texture to draw to
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param color Line color
 */
void graphics_draw_circle(texture_t* destination, int x, int y, int radius, color_t color);

/**
 * Draw circle with given pattern.
 *
 * @param destination Texture to draw to
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param pattern Texture to use as a pattern
 * @param offset_x Pattern x-axis offset
 * @param offset_y Pattern y-axis offset
 */
void graphics_draw_pattern_circle(texture_t* destination, int x, int y, int radius, texture_t* pattern, int pattern_offset_x, int pattern_offset_y);

/**
 * Draw filled circle.
 *
 * @param destination Texture to draw to
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param color Fill color
 */
void graphics_draw_filled_circle(texture_t* destination, int x, int y, int radius, color_t color);

/**
 * Draw filled circle with given pattern.
 *
 * @param destination Texture to draw to
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param pattern Texture to use as a pattern
 * @param offset_x Pattern x-axis offset
 * @param offset_y Pattern y-axis offset
 */
void graphics_draw_filled_pattern_circle(texture_t* destination, int x, int y, int radius, texture_t* pattern, int pattern_offset_x, int pattern_offset_y);

/**
 * Draw text.
 *
 * @param destination Texture to draw to
 * @param message Text to draw
 * @param x Text top-left x-coordinate
 * @param y Text top-left y-coordinate
 */
void graphics_draw_text(texture_t* destination, const char* message, int x, int y);

/**
 * Draw triangle.
 *
 * @param destination Texture to draw to
 * @param x0 Vertex 0 x-coordinate
 * @param y0 Vertex 0 y-coordinate
 * @param x1 Vertex 1 x-coordinate
 * @param y1 Vertex 1 y-coordinate
 * @param x2 Vertex 2 x-coordinate
 * @param y2 Vertex 2 y-coordinate
 * @param color Line color
 */
void graphics_draw_triangle(texture_t* destination, int x0, int y0, int x1, int y1, int x2, int y2, color_t color);

/**
 * Draw triangle with given pattern.
 *
 * @param destination Texture to draw to
 * @param x0 Vertex 0 x-coordinate
 * @param y0 Vertex 0 y-coordinate
 * @param x1 Vertex 1 x-coordinate
 * @param y1 Vertex 1 y-coordinate
 * @param x2 Vertex 2 x-coordinate
 * @param y2 Vertex 2 y-coordinate
 * @param pattern Texture to use as a pattern
 * @param offset_x Pattern x-axis offset
 * @param offset_y Pattern y-axis offset
 */
void graphics_draw_pattern_triangle(texture_t* destination, int x0, int y0, int x1, int y1, int x2, int y2, texture_t* pattern, int pattern_offset_x, int pattern_offset_y);

/**
 * Draw filled triangle.
 *
 * @param destination Texture to draw to
 * @param x0 Vertex 0 x-coordinate
 * @param y0 Vertex 0 y-coordinate
 * @param x1 Vertex 1 x-coordinate
 * @param y1 Vertex 1 y-coordinate
 * @param x2 Vertex 2 x-coordinate
 * @param y2 Vertex 2 y-coordinate
 * @param color_t Fill color
 */
void graphics_draw_filled_triangle(texture_t* destination, int x0, int y0, int x1, int y1, int x2, int y2, color_t color);

/**
 * Draw filled triangle with given pattern.
 *
 * @param destination Texture to draw to
 * @param x0 Vertex 0 x-coordinate
 * @param y0 Vertex 0 y-coordinate
 * @param x1 Vertex 1 x-coordinate
 * @param y1 Vertex 1 y-coordinate
 * @param x2 Vertex 2 x-coordinate
 * @param y2 Vertex 2 y-coordinate
 * @param pattern Texture to use as a pattern
 * @param offset_x Pattern x-axis offset
 * @param offset_y Pattern y-axis offset
 */
void graphics_draw_filled_pattern_triangle(texture_t* destination, int x0, int y0, int x1, int y1, int x2, int y2, texture_t* pattern, int pattern_offset_x, int pattern_offset_y);

/**
 * Draw textured triangle using affine texture mapping.
 *
 * @param destination Texture to draw to
 * @param x0 Vertex 0 x-coordinate
 * @param y0 Vertex 0 y-coordinate
 * @param u0 UV 0 u-coordinate
 * @param v0 UV 0 v-coordinate
 * @param x1 Vertex 1 x-coordinate
 * @param y1 Vertex 1 y-coordinate
 * @param u1 UV 1 u-coordinate
 * @param v1 UV 1 v-coordinate
 * @param x2 Vertex 2 x-coordinate
 * @param y2 Vertex 2 y-coordinate
 * @param u2 UV 2 u-coordinate
 * @param v2 UV 2 v-coordinate
 * @param texture_map Texture to
 */
void graphics_draw_textured_triangle(texture_t* destination, int x0, int y0, float u0, float v0, int x1, int y1, float u1, float v1, int x2, int y2, float u2, float v2, texture_t* texture_map);

/**
 * Draw source texture to destination texture.
 *
 * @param destination Texture to draw to
 * @param source Texture to draw
 * @param x Source x-coordinate on destination
 * @param y Source y-coordinate on destination
 * @param width Source width
 * @param height Source height
 */
void graphics_draw_texture(texture_t* destination, texture_t* source, int x, int y, int width, int height);

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
void graphics_draw_palette_reset(void);

/**
 * Set transparent color.
 *
 * @param color Color to set as transparent. A value of -1 is no transparency.
 */
void graphics_draw_transparent_color_set(int color);

/**
 * Get transparent color.
 *
 * @return int Transparent color. A value of -1 is no transparency.
 */
int graphics_draw_transparent_color_get(void);

#endif
