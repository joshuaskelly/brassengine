#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "draw.h"
#include "../assets.h"
#include "../graphics.h"
#include "../log.h"

static int mod(int a, int b) {
    return a - floor(a / (float)b) * b;
}

static void pattern_set_pixel(int x, int y, texture_t* pattern, int offset_x, int offset_y) {
    if (!pattern) return;

    int sx = mod(x - offset_x, pattern->width);
    int sy = mod(y - offset_y, pattern->height);

    color_t pixel = graphics_texture_get_pixel(pattern, sx, sy);
    color_t* draw_palette = graphics_draw_palette_get();
    pixel = draw_palette[pixel];

    graphics_set_pixel(x, y, pixel);
}

void draw_line(int x0, int y0, int x1, int y1, color_t color) {
    // DDA based line drawing algorithm
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;
    int longest_side = fmax(abs(delta_x), abs(delta_y));

    float x_inc = delta_x / (float)longest_side;
    float y_inc = delta_y / (float)longest_side;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= longest_side; i++) {
        graphics_set_pixel(current_x, current_y, color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void draw_pattern_line(int x0, int y0, int x1, int y1, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    // DDA based line drawing algorithm
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;
    int longest_side = fmax(abs(delta_x), abs(delta_y));

    float x_inc = delta_x / (float)longest_side;
    float y_inc = delta_y / (float)longest_side;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= longest_side; i++) {
        pattern_set_pixel(current_x, current_y, pattern, pattern_offset_x, pattern_offset_y);
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

void draw_pattern_rectangle(int x, int y, int width, int height, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    int x0 = x;
    int y0 = y;
    int x1 = x + width;
    int y1 = y + height;

    draw_pattern_line(x0, y0, x1, y0, pattern, pattern_offset_x, pattern_offset_y);
    draw_pattern_line(x1, y0, x1, y1, pattern, pattern_offset_x, pattern_offset_y);
    draw_pattern_line(x1, y1, x0, y1, pattern, pattern_offset_x, pattern_offset_y);
    draw_pattern_line(x0, y1, x0, y0, pattern, pattern_offset_x, pattern_offset_y);
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

void draw_filled_pattern_rectangle(int x, int y, int width, int height, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    int x0 = x;
    int x1 = x + width;
    int y0 = y;

    for (int i = 0; i < height; i++) {
        y0 = y + i;
        draw_pattern_line(x0, y0, x1, y0, pattern, pattern_offset_x, pattern_offset_y);
    }
}

/**
 * Plot 8 pixels of the circle at a time using octave symmetry.
 *
 * @param x Current x-coordinate on perimeter of circle
 * @param y Current y-coordinate on perimeter of circle
 * @param offset_x X-coordinate offset
 * @param offset_y Y-coordinate offset
 * @param color Line color
 */
static void draw_pixel_octave_symmetry(int x, int y, int offset_x, int offset_y, color_t color) {
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
 * Draw four horizontal lines at time using octave symmetry.
 *
 * @param x Current x-coordinate on perimeter of circle
 * @param y Current y-coordinate on perimeter of circle
 * @param offset_x X-coordinate offset
 * @param offset_y Y-coordinate offset
 * @param color Fill color
 */
static void fill_pixel_octave_symmetry(int x, int y, int offset_x, int offset_y, color_t color) {
    draw_line( x + offset_x,  y + offset_y, -x + offset_x,  y + offset_y, color);
    draw_line( y + offset_x,  x + offset_y, -y + offset_x,  x + offset_y, color);
    draw_line( x + offset_x, -y + offset_y, -x + offset_x, -y + offset_y, color);
    draw_line( y + offset_x, -x + offset_y, -y + offset_x, -x + offset_y, color);
}

/**
 * Draw circle
 *
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param color Line color
 */
void draw_circle(int x, int y, int radius, color_t color) {
    // Bresenham's circle algorithm
    if (radius <= 0) return;

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

static void draw_pattern_octave_symmetry(int x, int y, int offset_x, int offset_y, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    pattern_set_pixel( x + offset_x,  y + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    pattern_set_pixel( y + offset_x,  x + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    pattern_set_pixel(-x + offset_x,  y + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    pattern_set_pixel(-y + offset_x,  x + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    pattern_set_pixel( x + offset_x, -y + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    pattern_set_pixel( y + offset_x, -x + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    pattern_set_pixel(-x + offset_x, -y + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    pattern_set_pixel(-y + offset_x, -x + offset_y, pattern, pattern_offset_x, pattern_offset_y);
}

void draw_pattern_circle(int x, int y, int radius, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    // Bresenham's circle algorithm
    if (radius <= 0) return;

    int _x = 0;
    int _y = radius;
    int midpoint_criteria = 1 - radius;

    draw_pattern_octave_symmetry(_x, _y, x, y, pattern, pattern_offset_x, pattern_offset_y);

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
        draw_pattern_octave_symmetry(_x, _y, x, y, pattern, pattern_offset_x, pattern_offset_y);
    }
}

/**
 * Draw filled circle
 *
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param color Fill color
 */
void draw_filled_circle(int x, int y, int radius, color_t color) {
    // Bresenham's circle algorithm
    if (radius <= 0) return;

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

static void fill_pattern_octave_symmetry(int x, int y, int offset_x, int offset_y, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    draw_pattern_line( x + offset_x,  y + offset_y, -x + offset_x,  y + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    draw_pattern_line( y + offset_x,  x + offset_y, -y + offset_x,  x + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    draw_pattern_line( x + offset_x, -y + offset_y, -x + offset_x, -y + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    draw_pattern_line( y + offset_x, -x + offset_y, -y + offset_x, -x + offset_y, pattern, pattern_offset_x, pattern_offset_y);
}

void draw_filled_pattern_circle(int x, int y, int radius, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    // Bresenham's circle algorithm
    if (radius <= 0) return;

    int _x = 0;
    int _y = radius;
    int midpoint_criteria = 1 - radius;

    fill_pattern_octave_symmetry(_x, _y, x, y, pattern, pattern_offset_x, pattern_offset_y);

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
        fill_pattern_octave_symmetry(_x, _y, x, y, pattern, pattern_offset_x, pattern_offset_y);
    }
}

void draw_text(const char* message, int x, int y) {
    texture_t* font_texture = assets_get_texture("font.gif");
    if (!font_texture) {
        log_fatal("Missing font.gif asset");
    }

    rect_t source_rect = {0, 0, 8, 8};
    rect_t dest_rect = {0, y, 8, 8};

    int dest_x = x;

    for (int i = 0; i < strlen(message); i++) {
        unsigned char c = message[i];

        if (c == '\n') {
            dest_x = x;
            dest_rect.y += 8;
            continue;
        }

        if (c == '\t') {
            dest_x += 16;
            continue;
        }

        int cx = c % (font_texture->width / 8) * 8;
        int cy = c / (font_texture->width / 8) * 8;

        source_rect.x = cx;
        source_rect.y = cy;
        dest_rect.x = dest_x;

        dest_x += 8;

        graphics_blit(
            font_texture,
            NULL,
            &source_rect,
            &dest_rect,
            NULL
        );
    }
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

void draw_pattern_triangle(int x0, int y0, int x1, int y1, int x2, int y2, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    draw_pattern_line(x0, y0, x1, y1, pattern, pattern_offset_x, pattern_offset_y);
    draw_pattern_line(x1, y1, x2, y2, pattern, pattern_offset_x, pattern_offset_y);
    draw_pattern_line(x2, y2, x0, y0, pattern, pattern_offset_x, pattern_offset_y);
}
