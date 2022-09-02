#include <math.h>
#include <stdlib.h>

#include "draw.h"
#include "../graphics.h"

void draw_line(texture_t* texture, int x0, int y0, int x1, int y1, color_t color) {
    // DDA based line drawing algorithm

    int delta_x = x1 - x0;
    int delta_y = y1 - y0;
    int longest_side = fmax(abs(delta_x), abs(delta_y));

    float x_inc = delta_x / (float)longest_side;
    float y_inc = delta_y / (float)longest_side;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i < longest_side; i++) {
        texture_set_pixel(texture, current_x, current_y, color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void draw_rectangle(texture_t* texture, int x, int y, int width, int height, color_t color) {
    int x0 = x;
    int y0 = y;
    int x1 = x + width;
    int y1 = y + height;

    draw_line(texture, x0, y0, x1, y0, color);
    draw_line(texture, x1, y0, x1, y1, color);
    draw_line(texture, x1, y1, x0, y1, color);
    draw_line(texture, x0, y1, x0, y0, color);
}

void draw_filled_rectangle(texture_t* texture, int x, int y, int width, int height, color_t color) {
    int x0 = x;
    int x1 = x + width;
    int y0 = y;

    for (int i = 0; i < height; i++) {
        y0 = y + i;
        draw_line(texture, x0, y0, x1, y0, color);
    }
}
