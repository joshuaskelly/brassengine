#ifndef RENDERERS_DRAW_H
#define RENDERERS_DRAW_H

#include "../graphics.h"

/**
 * Draw line from x0, y0 to x1, y1.
 *
 * @param x0 Start x-coordinate
 * @param y0 Start y-coordinate
 * @param x1 End x-coordinate
 * @param y1 End y-coordinate
 * @param color Line color
 */
void draw_line(int x0, int y0, int x1, int y1, color_t color);

/**
 * Draw line from x0, y0 to x1, y1 with given pattern.
 *
 * @param x0 Start x-coordinate
 * @param y0 Start y-coordinate
 * @param x1 End x-coordinate
 * @param y1 End y-coordinate
 * @param pattern Texture to use as a pattern
 * @param offset_x Pattern x-axis offset
 * @param offset_y Pattern y-axis offset
 */
void draw_pattern_line(int x0, int y0, int x1, int y1, texture_t* pattern, int pattern_offset_x, int pattern_offset_y);

/**
 * Draw rectangle.
 *
 * @param x Rect top left x-coordinate
 * @param y Rect top left y-coordinate
 * @param width Rect width
 * @param height Rect height
 * @param color Line color
 */
void draw_rectangle(int x, int y, int width, int height, color_t color);

/**
 * Draw rectangle with given pattern.
 *
 * @param x Rect top left x-coordinate
 * @param y Rect top left y-coordinate
 * @param width Rect width
 * @param height Rect height
 * @param pattern Texture to use as a pattern
 * @param offset_x Pattern x-axis offset
 * @param offset_y Pattern y-axis offset
 */
void draw_pattern_rectangle(int x, int y, int width, int height, texture_t* pattern, int pattern_offset_x, int pattern_offset_y);

/**
 * Draw filled rectangle.
 *
 * @param x Rect top left x-coordinate
 * @param y Rect top left y-coordinate
 * @param width Rect width
 * @param height Rect height
 * @param color Fill color
 */
void draw_filled_rectangle(int x, int y, int width, int height, color_t color);

/**
 * Draw filled rectangle with given pattern.
 *
 * @param x Rect top left x-coordinate
 * @param y Rect top left y-coordinate
 * @param width Rect width
 * @param height Rect height
 * @param pattern Texture to use as a pattern
 * @param offset_x Pattern x-axis offset
 * @param offset_y Pattern y-axis offset
 */
void draw_filled_pattern_rectangle(int x, int y, int width, int height, texture_t* pattern, int pattern_offset_x, int pattern_offset_y);

/**
 * Draw circle.
 *
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param color Line color
 */
void draw_circle(int x, int y, int radius, color_t color);

/**
 * Draw circle with given pattern.
 *
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param pattern Texture to use as a pattern
 * @param offset_x Pattern x-axis offset
 * @param offset_y Pattern y-axis offset
 */
void draw_pattern_circle(int x, int y, int radius, texture_t* pattern, int pattern_offset_x, int pattern_offset_y);

/**
 * Draw filled circle.
 *
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param color Fill color
 */
void draw_filled_circle(int x, int y, int radius, color_t color);

/**
 * Draw filled circle with given pattern.
 *
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param pattern Texture to use as a pattern
 * @param offset_x Pattern x-axis offset
 * @param offset_y Pattern y-axis offset
 */
void draw_filled_pattern_circle(int x, int y, int radius, texture_t* pattern, int pattern_offset_x, int pattern_offset_y);

/**
 * Draw text.
 *
 * @param message Text to draw
 * @param x Text top-left x-coordinate
 * @param y Text top-left y-coordinate
 */
void draw_text(const char* message, int x, int y);

/**
 * Draw triangle.
 *
 * @param x0 Vertex 0 x-coordinate
 * @param y0 Vertex 0 y-coordinate
 * @param x1 Vertex 1 x-coordinate
 * @param y1 Vertex 1 y-coordinate
 * @param x2 Vertex 2 x-coordinate
 * @param y2 Vertex 2 y-coordinate
 * @param color Line color
 */
void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color);

/**
 * Draw triangle with given pattern.
 *
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
void draw_pattern_triangle(int x0, int y0, int x1, int y1, int x2, int y2, texture_t* pattern, int pattern_offset_x, int pattern_offset_y);

/**
 * Draw textured triangle using affine texture mapping.
 *
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
 * @param texture Texture to
 */
void draw_textured_triangle(int x0, int y0, float u0, float v0, int x1, int y1, float u1, float v1, int x2, int y2, float u2, float v2, texture_t* texture);

#endif
