#include <math.h>
#include <stdlib.h>

#include "draw.h"
#include "../graphics.h"

void draw_line(int x0, int y0, int x1, int y1, color_t color) {
    // DDA based line drawing algorithm
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;
    int longest_side = fmax(abs(delta_x), abs(delta_y));

    float x_inc = delta_x / (float)longest_side;
    float y_inc = delta_y / (float)longest_side;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i < longest_side; i++) {
        graphics_set_pixel(current_x, current_y, color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void draw_rectangle(int x, int y, int width, int height, color_t color) {
    int x0 = x;
    int y0 = y;
    int x1 = x + width;
    int y1 = y + height;

    draw_line(x0, y0, x1, y0, color);
    draw_line(x1, y0, x1, y1, color);
    draw_line(x1, y1, x0, y1, color);
    draw_line(x0, y1, x0, y0, color);
}

void draw_filled_rectangle(int x, int y, int width, int height, color_t color) {
    int x0 = x;
    int x1 = x + width;
    int y0 = y;

    for (int i = 0; i < height; i++) {
        y0 = y + i;
        draw_line(x0, y0, x1, y0, color);
    }
}

/**
 * @brief Plot 8 pixels of the circle at a time using octave symmetry.
 *
 * @param x Current x-coordinate on perimeter of circle
 * @param y Current y-coordinate on perimeter of circle
 * @param offset_x X-coordinate offset
 * @param offset_y Y-coordinate offset
 * @param color Line color
 */
void draw_pixel_octave_symmetry(int x, int y, int offset_x, int offset_y, color_t color) {
    graphics_set_pixel( x + offset_x,  y + offset_y, color);
    graphics_set_pixel( y + offset_x,  x + offset_y, color);
    graphics_set_pixel(-x + offset_x,  y + offset_y, color);
    graphics_set_pixel(-y + offset_x,  x + offset_y, color);
    graphics_set_pixel( x + offset_x, -y + offset_y, color);
    graphics_set_pixel( y + offset_x, -x + offset_y, color);
    graphics_set_pixel(-x + offset_x, -y + offset_y, color);
    graphics_set_pixel(-y + offset_x, -x + offset_y, color);
}

/**
 * @brief Draw four horizontal lines at time using octave symmetry.
 *
 * @param x Current x-coordinate on perimeter of circle
 * @param y Current y-coordinate on perimeter of circle
 * @param offset_x X-coordinate offset
 * @param offset_y Y-coordinate offset
 * @param color Fill color
 */
void fill_pixel_octave_symmetry(int x, int y, int offset_x, int offset_y, color_t color) {
    draw_line( x + offset_x,  y + offset_y, -x + offset_x,  y + offset_y, color);
    draw_line( y + offset_x,  x + offset_y, -y + offset_x,  x + offset_y, color);
    draw_line( x + offset_x, -y + offset_y, -x + offset_x, -y + offset_y, color);
    draw_line( y + offset_x, -x + offset_y, -y + offset_x, -x + offset_y, color);
}

void draw_circle(int x, int y, int radius, color_t color) {
    // Bresenham's circle algorithm
    int _x = 0;
    int _y = radius;
    int midpoint_criteria = 1 - radius;

    draw_pixel_octave_symmetry(_x, _y, x, y, color);

    while (_x < _y) {
        // Mid-point on or inside radius
        if (midpoint_criteria <= 0) {
            midpoint_criteria += (_x << 1) + 3;
        }
        // Mid-point outside radius
        else {
            midpoint_criteria += ((_x - _y) << 1) + 5;
            _y -= 1;
        }
        _x++;
        draw_pixel_octave_symmetry(_x, _y, x, y, color);
    }
}

void draw_filled_circle(int x, int y, int radius, color_t color) {
    // Bresenham's circle algorithm
    int _x = 0;
    int _y = radius;
    int midpoint_criteria = 1 - radius;

    fill_pixel_octave_symmetry(_x, _y, x, y, color);

    while (_x < _y) {
        // Mid-point on or inside radius
        if (midpoint_criteria <= 0) {
            midpoint_criteria += (_x << 1) + 3;
        }
        // Mid-point outside radius
        else {
            midpoint_criteria += ((_x - _y) << 1) + 5;
            _y -= 1;
        }
        _x++;
        fill_pixel_octave_symmetry(_x, _y, x, y, color);
    }
}
