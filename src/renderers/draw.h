#ifndef DRAW_H
#define DRAW_H

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
 * Draw circle.
 *
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param color Line color
 */
void draw_circle(int x, int y, int radius, color_t color);

/**
 * Draw filled circle.
 *
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param color Fill color
 */
void draw_filled_circle(int x, int y, int radius, color_t color);

#endif
