#ifndef DRAW_H
#define DRAW_H

#include "../graphics.h"

/**
 * @brief Draw line from x0, y0 to x1, y1
 *
 * @param texture Texture to draw line on
 * @param x0 Start x-coordinate
 * @param y0 Start y-coordinate
 * @param x1 End x-coordinate
 * @param y1 End y-coordinate
 * @param color Line color
 */
void draw_line(texture_t* texture, int x0, int y0, int x1, int y1, color_t color);

void draw_rectangle(texture_t* texture, int x, int y, int width, int height, color_t color);

void draw_filled_rectangle(texture_t* texture, int x, int y, int width, int height, color_t color);

#endif