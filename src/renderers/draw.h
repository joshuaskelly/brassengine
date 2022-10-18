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

#endif
