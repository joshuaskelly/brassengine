#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#include <mathc/mathc.h>

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

void draw_textured_line(int x0, int y0, float u0, float v0, int x1, int y1, float u1, float v1, texture_t* texture) {
    // DDA based line drawing algorithm
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;
    int longest_side = fmax(abs(delta_x), abs(delta_y));

    float x_inc = delta_x / (float)longest_side;
    float y_inc = delta_y / (float)longest_side;

    float current_x = x0;
    float current_y = y0;

    float s0 = u0 * texture->width;
    float t0 = v0 * texture->height;
    float s1 = u1 * texture->width;
    float t1 = v1 * texture->height;

    float delta_s = s1 - s0;
    float delta_t = t1 - t0;
    float st_longest_side = fmax(abs(delta_s), abs(delta_t));

    float r = st_longest_side / longest_side;

    float s_inc = delta_s / st_longest_side * r;
    float t_inc = delta_t / st_longest_side * r;

    float current_s = s0;
    float current_t = t0;

    for (int i = 0; i <= longest_side; i++) {
        color_t c = graphics_texture_get_pixel(texture, current_s, current_t);

        graphics_set_pixel(current_x, current_y, c);

        current_x += x_inc;
        current_y += y_inc;
        current_s += s_inc;
        current_t += t_inc;
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

static mfloat_t vec2_cross(mfloat_t* v0, mfloat_t* v1) {
    return v0[0] * v1[1] - v0[1] * v1[0];
}

static int edge_function(mfloat_t* a, mfloat_t* b, mfloat_t* p) {
    mfloat_t ab[VEC2_SIZE];
    vec2_subtract(ab, b, a);

    mfloat_t ap[VEC2_SIZE];
    vec2_subtract(ap, p, a);

    return vec2_cross(ab, ap);
}

static bool is_top_left(mfloat_t* a, mfloat_t* b) {
    mfloat_t edge[VEC2_SIZE];
    vec2_subtract(edge, b, a);

    bool is_top = edge[1] == 0 && edge[0] > 0;
    bool is_left = edge[1] < 0;

    return is_top || is_left;
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color) {
    mfloat_t vertex0[VEC2_SIZE] = {x0, y0};
    mfloat_t vertex1[VEC2_SIZE] = {x1, y1};
    mfloat_t vertex2[VEC2_SIZE] = {x2, y2};

    // Find triangle
    int x_min = fminf(fminf(vertex0[0], vertex1[0]), vertex2[0]);
    int y_min = fminf(fminf(vertex0[1], vertex1[1]), vertex2[1]);
    int x_max = fmaxf(fmaxf(vertex0[0], vertex1[0]), vertex2[0]);
    int y_max = fmaxf(fmaxf(vertex0[1], vertex1[1]), vertex2[1]);

    // Biases for fill rule
    float bias0 = is_top_left(vertex1, vertex2) ? 0.0f : -0.001f;
    float bias1 = is_top_left(vertex2, vertex0) ? 0.0f : -0.001f;
    float bias2 = is_top_left(vertex0, vertex1) ? 0.0f : -0.001f;

    float delta_w0_col = vertex1[1] - vertex2[1];
    float delta_w1_col = vertex2[1] - vertex0[1];
    float delta_w2_col = vertex0[1] - vertex1[1];
    float delta_w0_row = vertex2[0] - vertex1[0];
    float delta_w1_row = vertex0[0] - vertex2[0];
    float delta_w2_row = vertex1[0] - vertex0[0];

    mfloat_t p[2] = { x_min + 0.5f, y_min + 0.5f };
    float w0_row = edge_function(vertex1, vertex2, p) + bias0;
    float w1_row = edge_function(vertex2, vertex0, p) + bias1;
    float w2_row = edge_function(vertex0, vertex1, p) + bias2;

    for (int y = y_min; y <= y_max; y++) {
        float w0 = w0_row;
        float w1 = w1_row;
        float w2 = w2_row;

        for (int x = x_min; x <= x_max; x++) {
            // Check if inside the triangle
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                graphics_set_pixel(x, y, color);
            }

            w0 += delta_w0_col;
            w1 += delta_w1_col;
            w2 += delta_w2_col;
        }

        w0_row += delta_w0_row;
        w1_row += delta_w1_row;
        w2_row += delta_w2_row;
    }
}

void draw_filled_pattern_triangle(int x0, int y0, int x1, int y1, int x2, int y2, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    mfloat_t vertex0[VEC2_SIZE] = {x0, y0};
    mfloat_t vertex1[VEC2_SIZE] = {x1, y1};
    mfloat_t vertex2[VEC2_SIZE] = {x2, y2};

    // Find triangle
    int x_min = fminf(fminf(vertex0[0], vertex1[0]), vertex2[0]);
    int y_min = fminf(fminf(vertex0[1], vertex1[1]), vertex2[1]);
    int x_max = fmaxf(fmaxf(vertex0[0], vertex1[0]), vertex2[0]);
    int y_max = fmaxf(fmaxf(vertex0[1], vertex1[1]), vertex2[1]);

    // Biases for fill rule
    float bias0 = is_top_left(vertex1, vertex2) ? 0.0f : -0.001f;
    float bias1 = is_top_left(vertex2, vertex0) ? 0.0f : -0.001f;
    float bias2 = is_top_left(vertex0, vertex1) ? 0.0f : -0.001f;

    float delta_w0_col = vertex1[1] - vertex2[1];
    float delta_w1_col = vertex2[1] - vertex0[1];
    float delta_w2_col = vertex0[1] - vertex1[1];
    float delta_w0_row = vertex2[0] - vertex1[0];
    float delta_w1_row = vertex0[0] - vertex2[0];
    float delta_w2_row = vertex1[0] - vertex0[0];

    mfloat_t p[2] = { x_min + 0.5f, y_min + 0.5f };
    float w0_row = edge_function(vertex1, vertex2, p) + bias0;
    float w1_row = edge_function(vertex2, vertex0, p) + bias1;
    float w2_row = edge_function(vertex0, vertex1, p) + bias2;

    for (int y = y_min; y <= y_max; y++) {
        float w0 = w0_row;
        float w1 = w1_row;
        float w2 = w2_row;

        for (int x = x_min; x <= x_max; x++) {
            // Check if inside the triangle
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                pattern_set_pixel(x, y, pattern, pattern_offset_x, pattern_offset_y);
            }

            w0 += delta_w0_col;
            w1 += delta_w1_col;
            w2 += delta_w2_col;
        }

        w0_row += delta_w0_row;
        w1_row += delta_w1_row;
        w2_row += delta_w2_row;
    }
}

void draw_textured_triangle(int x0, int y0, float u0, float v0, int x1, int y1, float u1, float v1, int x2, int y2, float u2, float v2, texture_t* texture) {
    mfloat_t vertex0[VEC2_SIZE] = {x0, y0};
    mfloat_t vertex1[VEC2_SIZE] = {x1, y1};
    mfloat_t vertex2[VEC2_SIZE] = {x2, y2};
    mfloat_t uv0[VEC2_SIZE] = {u0, v0};
    mfloat_t uv1[VEC2_SIZE] = {u1, v1};
    mfloat_t uv2[VEC2_SIZE] = {u2, v2};

    // Find triangle
    int x_min = fminf(fminf(vertex0[0], vertex1[0]), vertex2[0]);
    int y_min = fminf(fminf(vertex0[1], vertex1[1]), vertex2[1]);
    int x_max = fmaxf(fmaxf(vertex0[0], vertex1[0]), vertex2[0]);
    int y_max = fmaxf(fmaxf(vertex0[1], vertex1[1]), vertex2[1]);

    // Biases for fill rule
    float bias0 = is_top_left(vertex1, vertex2) ? 0.0f : -0.001f;
    float bias1 = is_top_left(vertex2, vertex0) ? 0.0f : -0.001f;
    float bias2 = is_top_left(vertex0, vertex1) ? 0.0f : -0.001f;

    float delta_w0_col = vertex1[1] - vertex2[1];
    float delta_w1_col = vertex2[1] - vertex0[1];
    float delta_w2_col = vertex0[1] - vertex1[1];
    float delta_w0_row = vertex2[0] - vertex1[0];
    float delta_w1_row = vertex0[0] - vertex2[0];
    float delta_w2_row = vertex1[0] - vertex0[0];

    float area = edge_function(vertex1, vertex2, vertex0);
    float inverse_area = 1.0f / area;

    mfloat_t p[2] = { x_min + 0.5f, y_min + 0.5f };
    float w0_row = edge_function(vertex1, vertex2, p) + bias0;
    float w1_row = edge_function(vertex2, vertex0, p) + bias1;
    float w2_row = edge_function(vertex0, vertex1, p) + bias2;

    for (int y = y_min; y <= y_max; y++) {
        float w0 = w0_row;
        float w1 = w1_row;
        float w2 = w2_row;

        for (int x = x_min; x <= x_max; x++) {
            // Check if inside the triangle
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                // Calculate barycentric coords
                float alpha = w0 * inverse_area;
                float beta = w1 * inverse_area;
                float gamma = w2 * inverse_area;

                // Calculate st coords
                int s = (uv0[0] * alpha + uv1[0] * beta + uv2[0] * gamma) * texture->width;
                int t = (uv0[1] * alpha + uv1[1] * beta + uv2[1] * gamma) * texture->height;

                color_t c = graphics_texture_get_pixel(texture, s, t);

                graphics_set_pixel(x, y, c);
            }

            w0 += delta_w0_col;
            w1 += delta_w1_col;
            w2 += delta_w2_col;
        }

        w0_row += delta_w0_row;
        w1_row += delta_w1_row;
        w2_row += delta_w2_row;
    }
}
