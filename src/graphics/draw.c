#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <math.h>

#include <mathc/mathc.h>

#include "draw.h"
#include "../assets.h"
#include "../graphics.h"
#include "../log.h"
#include "../math.h"

static color_t draw_palette[256];
static color_t transparent_color = 0;
static rect_t clip_rect;

void graphics_draw_pixel(texture_t* destination, int x, int y, color_t color) {
    // Don't draw if transparent
    if (color == transparent_color) return;

    // Don't draw if outside clipping rectangle
    if (x < clip_rect.x || x >= clip_rect.x + clip_rect.width) return;
    if (y < clip_rect.y || y >= clip_rect.y + clip_rect.height) return;

    graphics_texture_pixel_set(destination, x, y, color);
}

static void pattern_pixel_set(texture_t* destination, int x, int y, texture_t* pattern, int offset_x, int offset_y) {
    if (!pattern) return;

    int sx = modulo(x - offset_x, graphics_texture_width_get(pattern));
    int sy = modulo(y - offset_y, graphics_texture_height_get(pattern));

    color_t pixel = graphics_texture_pixel_get(pattern, sx, sy);
    color_t* draw_palette = graphics_draw_palette_get();
    pixel = draw_palette[pixel];

    graphics_draw_pixel(destination, x, y, pixel);
}

void graphics_draw_line(texture_t* destination, int x0, int y0, int x1, int y1, color_t color) {
    // DDA based line drawing algorithm
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;
    int longest_side = fmax(abs(delta_x), abs(delta_y));

    float x_inc = delta_x / (float)longest_side;
    float y_inc = delta_y / (float)longest_side;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= longest_side; i++) {
        graphics_draw_pixel(destination, current_x, current_y, color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void graphics_draw_pattern_line(texture_t* destination, int x0, int y0, int x1, int y1, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    // DDA based line drawing algorithm
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;
    int longest_side = fmax(abs(delta_x), abs(delta_y));

    float x_inc = delta_x / (float)longest_side;
    float y_inc = delta_y / (float)longest_side;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= longest_side; i++) {
        pattern_pixel_set(destination, current_x, current_y, pattern, pattern_offset_x, pattern_offset_y);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void graphics_draw_textured_line(texture_t* destination, int x0, int y0, float u0, float v0, int x1, int y1, float u1, float v1, texture_t* texture_map) {
    // DDA based line drawing algorithm
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;
    int xy_longest_side = fmax(abs(delta_x), abs(delta_y));

    float x_inc = delta_x / (float)xy_longest_side;
    float y_inc = delta_y / (float)xy_longest_side;

    float current_x = x0;
    float current_y = y0;

    float texture_width = graphics_texture_width_get(texture_map) - 1;
    float texture_height = graphics_texture_height_get(texture_map) - 1;

    // Convert uv-coordinates to st-coordinates
    float s0 = u0 * texture_width;
    float t0 = v0 * texture_height;
    float s1 = u1 * texture_width;
    float t1 = v1 * texture_height;

    float delta_s = s1 - s0;
    float delta_t = t1 - t0;
    float st_longest_side = fmax(fabs(delta_s), fabs(delta_t));

    // Length of longest side in terms of pixels.
    float st_distance_pixels = st_longest_side + 1;
    float xy_distance_pixels = xy_longest_side + 1;

    // Calculate ratio of st-coordinates to xy-coordinates.
    float ratio = st_distance_pixels / xy_distance_pixels;

    float s_inc = delta_s / st_longest_side * ratio;
    float t_inc = delta_t / st_longest_side * ratio;

    // Sample texture at pixel centers
    float s_scaled_pixel_center = delta_s == 0 ? 0.5f : 0.5f * ratio;
    float t_scaled_pixel_center = delta_t == 0 ? 0.5f : 0.5f * ratio;
    float current_s = s0 + s_scaled_pixel_center;
    float current_t = t0 + t_scaled_pixel_center;

    for (int i = 0; i <= xy_longest_side; i++) {
        if (current_s >= 0 && current_t >= 0) {
            int s = floor(current_s);
            int t = floor(current_t);
            color_t c = graphics_texture_pixel_get(texture_map, s, t);

            graphics_draw_pixel(destination, current_x, current_y, c);
        }

        current_x += x_inc;
        current_y += y_inc;
        current_s += s_inc;
        current_t += t_inc;
    }
}

void graphics_draw_bezier(texture_t* destination, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, color_t color) {
    mfloat_t a[VEC2_SIZE] = {x0, y0};
    mfloat_t b[VEC2_SIZE] = {x1, y1};
    mfloat_t c[VEC2_SIZE] = {x2, y2};
    mfloat_t d[VEC2_SIZE] = {x3, y3};

    mfloat_t ab[VEC2_SIZE];
    mfloat_t bc[VEC2_SIZE];
    mfloat_t cd[VEC2_SIZE];

    mfloat_t abbc[VEC2_SIZE];
    mfloat_t bccd[VEC2_SIZE];

    mfloat_t first[VEC2_SIZE] = {x0, y0};
    mfloat_t next[VEC2_SIZE];

    // Approximate curve length
    mfloat_t d0 = vec2_distance(a, d);
    mfloat_t d1 = vec2_distance(a, b) + vec2_distance(b, c) + vec2_distance(c, d);
    mfloat_t length = 2.0f / 3.0f * d0 + 1.0f / 3.0f * d1;

    int pixels_per_segment = 4;
    int steps = length / pixels_per_segment;

    for (int i = 1; i <= steps; i++) {
        mfloat_t t = i / (float)steps;

        vec2_lerp(ab, a, b, t);
        vec2_lerp(bc, b, c, t);
        vec2_lerp(cd, c, d, t);

        vec2_lerp(abbc, ab, bc, t);
        vec2_lerp(bccd, bc, cd, t);

        vec2_lerp(next, abbc, bccd, t);

        graphics_draw_line(destination, first[0], first[1], next[0], next[1], color);

        vec2_assign(first, next);
    }
}

void graphics_draw_pattern_bezier(texture_t* destination, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    mfloat_t a[VEC2_SIZE] = {x0, y0};
    mfloat_t b[VEC2_SIZE] = {x1, y1};
    mfloat_t c[VEC2_SIZE] = {x2, y2};
    mfloat_t d[VEC2_SIZE] = {x3, y3};

    mfloat_t ab[VEC2_SIZE];
    mfloat_t bc[VEC2_SIZE];
    mfloat_t cd[VEC2_SIZE];

    mfloat_t abbc[VEC2_SIZE];
    mfloat_t bccd[VEC2_SIZE];

    mfloat_t first[VEC2_SIZE] = {x0, y0};
    mfloat_t next[VEC2_SIZE];

    // Approximate curve length
    mfloat_t d0 = vec2_distance(a, d);
    mfloat_t d1 = vec2_distance(a, b) + vec2_distance(b, c) + vec2_distance(c, d);
    mfloat_t length = 2.0f / 3.0f * d0 + 1.0f / 3.0f * d1;

    int pixels_per_segment = 4;
    int steps = length / pixels_per_segment;

    for (int i = 1; i <= steps; i++) {
        mfloat_t t = i / (float)steps;

        vec2_lerp(ab, a, b, t);
        vec2_lerp(bc, b, c, t);
        vec2_lerp(cd, c, d, t);

        vec2_lerp(abbc, ab, bc, t);
        vec2_lerp(bccd, bc, cd, t);

        vec2_lerp(next, abbc, bccd, t);

        graphics_draw_pattern_line(destination, first[0], first[1], next[0], next[1], pattern, pattern_offset_x, pattern_offset_y);

        vec2_assign(first, next);
    }
}

void graphics_draw_rectangle(texture_t* destination, int x, int y, int width, int height, color_t color) {
    int x0 = x;
    int y0 = y;
    int x1 = x + width - 1;
    int y1 = y + height - 1;

    graphics_draw_line(destination, x0, y0, x1, y0, color);
    graphics_draw_line(destination, x1, y0, x1, y1, color);
    graphics_draw_line(destination, x1, y1, x0, y1, color);
    graphics_draw_line(destination, x0, y1, x0, y0, color);
}

void graphics_draw_pattern_rectangle(texture_t* destination, int x, int y, int width, int height, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    int x0 = x;
    int y0 = y;
    int x1 = x + width - 1;
    int y1 = y + height - 1;

    graphics_draw_pattern_line(destination, x0, y0, x1, y0, pattern, pattern_offset_x, pattern_offset_y);
    graphics_draw_pattern_line(destination, x1, y0, x1, y1, pattern, pattern_offset_x, pattern_offset_y);
    graphics_draw_pattern_line(destination, x1, y1, x0, y1, pattern, pattern_offset_x, pattern_offset_y);
    graphics_draw_pattern_line(destination, x0, y1, x0, y0, pattern, pattern_offset_x, pattern_offset_y);
}

void graphics_draw_filled_rectangle(texture_t* destination, int x, int y, int width, int height, color_t color) {
    int x0 = x;
    int x1 = x + width - 1;
    int y0 = y;

    for (int i = 0; i < height; i++) {
        y0 = y + i;
        graphics_draw_line(destination, x0, y0, x1, y0, color);
    }
}

void graphics_draw_filled_pattern_rectangle(texture_t* destination, int x, int y, int width, int height, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    int x0 = x;
    int x1 = x + width - 1;
    int y0 = y;

    for (int i = 0; i < height; i++) {
        y0 = y + i;
        graphics_draw_pattern_line(destination, x0, y0, x1, y0, pattern, pattern_offset_x, pattern_offset_y);
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
static void draw_pixel_octave_symmetry(texture_t* destination, int x, int y, int offset_x, int offset_y, color_t color) {
    graphics_draw_pixel(destination,  x + offset_x,  y + offset_y, color);
    graphics_draw_pixel(destination,  y + offset_x,  x + offset_y, color);
    graphics_draw_pixel(destination, -x + offset_x,  y + offset_y, color);
    graphics_draw_pixel(destination, -y + offset_x,  x + offset_y, color);
    graphics_draw_pixel(destination,  x + offset_x, -y + offset_y, color);
    graphics_draw_pixel(destination,  y + offset_x, -x + offset_y, color);
    graphics_draw_pixel(destination, -x + offset_x, -y + offset_y, color);
    graphics_draw_pixel(destination, -y + offset_x, -x + offset_y, color);
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
static void fill_pixel_octave_symmetry(texture_t* destination, int x, int y, int offset_x, int offset_y, color_t color) {
    graphics_draw_line(destination, x + offset_x,  y + offset_y, -x + offset_x,  y + offset_y, color);
    graphics_draw_line(destination, y + offset_x,  x + offset_y, -y + offset_x,  x + offset_y, color);
    graphics_draw_line(destination, x + offset_x, -y + offset_y, -x + offset_x, -y + offset_y, color);
    graphics_draw_line(destination, y + offset_x, -x + offset_y, -y + offset_x, -x + offset_y, color);
}

/**
 * Draw circle
 *
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param color Line color
 */
void graphics_draw_circle(texture_t* destination, int x, int y, int radius, color_t color) {
    // Bresenham's circle algorithm
    if (radius <= 0) return;

    int _x = 0;
    int _y = radius;
    int midpoint_criteria = 1 - radius;

    draw_pixel_octave_symmetry(destination, _x, _y, x, y, color);

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
        draw_pixel_octave_symmetry(destination, _x, _y, x, y, color);
    }
}

static void draw_pattern_octave_symmetry(texture_t* destination, int x, int y, int offset_x, int offset_y, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    pattern_pixel_set(destination,  x + offset_x,  y + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    pattern_pixel_set(destination,  y + offset_x,  x + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    pattern_pixel_set(destination, -x + offset_x,  y + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    pattern_pixel_set(destination, -y + offset_x,  x + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    pattern_pixel_set(destination,  x + offset_x, -y + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    pattern_pixel_set(destination,  y + offset_x, -x + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    pattern_pixel_set(destination, -x + offset_x, -y + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    pattern_pixel_set(destination, -y + offset_x, -x + offset_y, pattern, pattern_offset_x, pattern_offset_y);
}

void graphics_draw_pattern_circle(texture_t* destination, int x, int y, int radius, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    // Bresenham's circle algorithm
    if (radius <= 0) return;

    int _x = 0;
    int _y = radius;
    int midpoint_criteria = 1 - radius;

    draw_pattern_octave_symmetry(destination, _x, _y, x, y, pattern, pattern_offset_x, pattern_offset_y);

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
        draw_pattern_octave_symmetry(destination, _x, _y, x, y, pattern, pattern_offset_x, pattern_offset_y);
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
void graphics_draw_filled_circle(texture_t* destination, int x, int y, int radius, color_t color) {
    // Bresenham's circle algorithm
    if (radius <= 0) return;

    int _x = 0;
    int _y = radius;
    int midpoint_criteria = 1 - radius;

    fill_pixel_octave_symmetry(destination, _x, _y, x, y, color);

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
        fill_pixel_octave_symmetry(destination, _x, _y, x, y, color);
    }
}

static void fill_pattern_octave_symmetry(texture_t* destination, int x, int y, int offset_x, int offset_y, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    graphics_draw_pattern_line(destination, x + offset_x,  y + offset_y, -x + offset_x,  y + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    graphics_draw_pattern_line(destination, y + offset_x,  x + offset_y, -y + offset_x,  x + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    graphics_draw_pattern_line(destination, x + offset_x, -y + offset_y, -x + offset_x, -y + offset_y, pattern, pattern_offset_x, pattern_offset_y);
    graphics_draw_pattern_line(destination, y + offset_x, -x + offset_y, -y + offset_x, -x + offset_y, pattern, pattern_offset_x, pattern_offset_y);
}

void graphics_draw_filled_pattern_circle(texture_t* destination, int x, int y, int radius, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    // Bresenham's circle algorithm
    if (radius <= 0) return;

    int _x = 0;
    int _y = radius;
    int midpoint_criteria = 1 - radius;

    fill_pattern_octave_symmetry(destination, _x, _y, x, y, pattern, pattern_offset_x, pattern_offset_y);

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
        fill_pattern_octave_symmetry(destination, _x, _y, x, y, pattern, pattern_offset_x, pattern_offset_y);
    }
}

static void draw_blit_func(texture_t* source_texture, texture_t* destination_texture, int sx, int sy, int dx, int dy) {
    color_t pixel = graphics_texture_pixel_get(source_texture, sx, sy);
    color_t* draw_palette = graphics_draw_palette_get();
    pixel = draw_palette[pixel];

    graphics_draw_pixel(destination_texture, dx, dy, pixel);
}

void graphics_draw_text(texture_t* destination, const char* message, int x, int y) {
    texture_t* font_texture = assets_texture_get("font.gif", 0);
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

        int cx = c % (graphics_texture_width_get(font_texture) / 8) * 8;
        int cy = c / (graphics_texture_width_get(font_texture) / 8) * 8;

        source_rect.x = cx;
        source_rect.y = cy;
        dest_rect.x = dest_x;

        dest_x += 8;

        graphics_blit(
            font_texture,
            destination,
            &source_rect,
            &dest_rect,
            draw_blit_func
        );
    }
}

void graphics_draw_triangle(texture_t* destination, int x0, int y0, int x1, int y1, int x2, int y2, color_t color) {
    graphics_draw_line(destination, x0, y0, x1, y1, color);
    graphics_draw_line(destination, x1, y1, x2, y2, color);
    graphics_draw_line(destination, x2, y2, x0, y0, color);
}

void graphics_draw_pattern_triangle(texture_t* destination, int x0, int y0, int x1, int y1, int x2, int y2, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    graphics_draw_pattern_line(destination, x0, y0, x1, y1, pattern, pattern_offset_x, pattern_offset_y);
    graphics_draw_pattern_line(destination, x1, y1, x2, y2, pattern, pattern_offset_x, pattern_offset_y);
    graphics_draw_pattern_line(destination, x2, y2, x0, y0, pattern, pattern_offset_x, pattern_offset_y);
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

void graphics_draw_filled_triangle(texture_t* destination, int x0, int y0, int x1, int y1, int x2, int y2, color_t color) {
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
                graphics_draw_pixel(destination, x, y, color);
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

void graphics_draw_filled_pattern_triangle(texture_t* destination, int x0, int y0, int x1, int y1, int x2, int y2, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
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
                pattern_pixel_set(destination, x, y, pattern, pattern_offset_x, pattern_offset_y);
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

void graphics_draw_textured_triangle(texture_t* destination, int x0, int y0, float u0, float v0, int x1, int y1, float u1, float v1, int x2, int y2, float u2, float v2, texture_t* texture_map) {
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
                int s = (uv0[0] * alpha + uv1[0] * beta + uv2[0] * gamma) * graphics_texture_width_get(texture_map);
                int t = (uv0[1] * alpha + uv1[1] * beta + uv2[1] * gamma) * graphics_texture_height_get(texture_map);

                color_t c = graphics_texture_pixel_get(texture_map, s, t);

                graphics_draw_pixel(destination, x, y, c);
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

void graphics_draw_quad(texture_t* destination, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, color_t color) {
    graphics_draw_line(destination, x0, y0, x1, y1, color);
    graphics_draw_line(destination, x1, y1, x2, y2, color);
    graphics_draw_line(destination, x2, y2, x3, y3, color);
    graphics_draw_line(destination, x3, y3, x0, y0, color);
}

void graphics_draw_pattern_quad(texture_t* destination, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    graphics_draw_pattern_line(destination, x0, y0, x1, y1, pattern, pattern_offset_x, pattern_offset_y);
    graphics_draw_pattern_line(destination, x1, y1, x2, y2, pattern, pattern_offset_x, pattern_offset_y);
    graphics_draw_pattern_line(destination, x2, y2, x3, y3, pattern, pattern_offset_x, pattern_offset_y);
    graphics_draw_pattern_line(destination, x3, y3, x0, y0, pattern, pattern_offset_x, pattern_offset_y);
}

static int compare_floats(const void* a, const void* b) {
    return *(const float*)a > *(const float*)b;
}

void graphics_draw_filled_quad(texture_t* destination, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, color_t color) {
    int min_y = y0;
    min_y = fminf(min_y, y1);
    min_y = fminf(min_y, y2);
    min_y = fminf(min_y, y3);

    int max_y = y0;
    max_y = fmaxf(max_y, y1);
    max_y = fmaxf(max_y, y2);
    max_y = fmaxf(max_y, y3);

    mfloat_t points[4][VEC2_SIZE] = {
        {x0, y0},
        {x1, y1},
        {x2, y2},
        {x3, y3}
    };

    float intersections[4];
    int count = 0;

    for (int y = min_y; y <= max_y; y++) {
        count = 0;

        for (int i = 0; i < 4; i++) {
            mfloat_t* a = points[i];
            mfloat_t* b = points[(i + 1) % 4];

            if (y <= a[1] && y <= b[1]) continue;
            if (y > a[1] && y > b[1]) continue;

            // Don't intersect horizontal lines
            if (approximately(a[1], b[1])) continue;

            mfloat_t d[VEC2_SIZE];
            vec2_subtract(d, b, a);
            float x = b[0] - (b[1] - y) / d[1] * d[0];

            intersections[count] = x;
            count++;
        }

        if (count == 0) continue;

        qsort(intersections, count, sizeof(float), compare_floats);

        for (int i = 0; i < count; i += 2) {
            float x0 = floorf(intersections[i]);
            float x1 = floorf(intersections[i + 1]);

            graphics_draw_line(destination, x0, y, x1, y, color);
        }
    }
}

void graphics_draw_filled_pattern_quad(texture_t* destination, int x0, int y0, int x1, int y1, int x2, int y2, int x3, int y3, texture_t* pattern, int pattern_offset_x, int pattern_offset_y) {
    int min_y = y0;
    min_y = fminf(min_y, y1);
    min_y = fminf(min_y, y2);
    min_y = fminf(min_y, y3);

    int max_y = y0;
    max_y = fmaxf(max_y, y1);
    max_y = fmaxf(max_y, y2);
    max_y = fmaxf(max_y, y3);

    mfloat_t points[4][VEC2_SIZE] = {
        {x0, y0},
        {x1, y1},
        {x2, y2},
        {x3, y3}
    };

    float intersections[4];
    int count = 0;

    for (int y = min_y; y <= max_y; y++) {
        count = 0;

        for (int i = 0; i < 4; i++) {
            mfloat_t* a = points[i];
            mfloat_t* b = points[(i + 1) % 4];

            if (y <= a[1] && y <= b[1]) continue;
            if (y > a[1] && y > b[1]) continue;

            // Don't intersect horizontal lines
            if (approximately(a[1], b[1])) continue;

            mfloat_t d[VEC2_SIZE];
            vec2_subtract(d, b, a);
            float x = b[0] - (b[1] - y) / d[1] * d[0];

            intersections[count] = x;
            count++;
        }

        if (count == 0) continue;

        qsort(intersections, count, sizeof(float), compare_floats);

        for (int i = 0; i < count; i += 2) {
            float x0 = floorf(intersections[i]);
            float x1 = floorf(intersections[i + 1]);

            graphics_draw_pattern_line(
                destination,
                x0, y,
                x1, y,
                pattern,
                pattern_offset_x,
                pattern_offset_y
            );
        }
    }
}

// Adapted from: https://www.reedbeta.com/blog/quadrilateral-interpolation-part-2/
static void inverse_bilinear(mfloat_t* result, mfloat_t* p, mfloat_t* a, mfloat_t* b, mfloat_t* c, mfloat_t* d, bool flip) {
    mfloat_t q[VEC2_SIZE];
    mfloat_t b1[VEC2_SIZE];
    mfloat_t b2[VEC2_SIZE];
    mfloat_t b3[VEC2_SIZE];

    vec2_subtract(q, p, a);
    vec2_subtract(b1, b, a);
    vec2_subtract(b2, d, a);

    // a - b - d + c
    vec2_subtract(b3, a, b);
    vec2_subtract(b3, b3, d);
    vec2_add(b3, b3, c);

    float A = vec2_cross(b2, b3);
    float B = vec2_cross(b3, q) - vec2_cross(b1, b2);
    float C = vec2_cross(b1, q);

    float discriminant = 0.0f;
    float sqrd = 0.0f;

    if (fabsf(A) < 0.0001f) {
        result[1] = -C / B;
    }
    else {
        discriminant = B * B - 4.0f * A * C;
        sqrd = sqrtf(discriminant);

        if (flip) {
            result[1] = 0.5f * (-B + sqrd) / A;
        }
        else {
            result[1] = 0.5f * (-B - sqrd) / A;
        }
    }

    mfloat_t denominator[VEC2_SIZE];
    mfloat_t bb3[VEC2_SIZE];
    vec2_multiply_f(bb3, b3, result[1]);
    vec2_add(denominator, b1, bb3);

    if (fabsf(denominator[0]) > fabsf(denominator[1])) {
        result[0] = (q[0] - b2[0] * result[1]) / denominator[0];
    }
    else {
        result[0] = (q[1] - b2[1] * result[1]) / denominator[1];
    }
}

typedef struct intersection {
    float x;
    int index;
} intersection_t;

static int compare_intersections(const void* a, const void* b) {
    intersection_t* A = (intersection_t*)a;
    intersection_t* B = (intersection_t*)b;

    return A->x > B->x;
}

void graphics_draw_textured_quad(texture_t* destination, int x0, int y0, float u0, float v0, int x1, int y1, float u1, float v1, int x2, int y2, float u2, float v2, int x3, int y3, float u3, float v3, texture_t* texture_map) {
    int min_y = y0;
    min_y = fminf(min_y, y1);
    min_y = fminf(min_y, y2);
    min_y = fminf(min_y, y3);

    int max_y = y0;
    max_y = fmaxf(max_y, y1);
    max_y = fmaxf(max_y, y2);
    max_y = fmaxf(max_y, y3);

    mfloat_t points[4][VEC2_SIZE] = {
        {x0, y0},
        {x1, y1},
        {x2, y2},
        {x3, y3}
    };

    mfloat_t edge_vectors[4][VEC2_SIZE];
    vec2_subtract(edge_vectors[0], points[1], points[0]);
    vec2_subtract(edge_vectors[1], points[2], points[1]);
    vec2_subtract(edge_vectors[2], points[3], points[2]);
    vec2_subtract(edge_vectors[3], points[0], points[3]);

    intersection_t intersections[4];
    int count = 0;

    for (int y = min_y; y <= max_y; y++) {
        count = 0;

        // Intersect scanline with edges
        for (int i = 0; i < 4; i++) {
            mfloat_t* a = points[i];
            mfloat_t* b = points[(i + 1) % 4];

            if (y <= a[1] && y <= b[1]) continue;
            if (y > a[1] && y > b[1]) continue;

            mfloat_t d[VEC2_SIZE];
            vec2_subtract(d, b, a);

            float x = b[0] - (b[1] - y) / d[1] * d[0];

            intersections[count].x = x;
            intersections[count].index = i;
            count++;
        }

        if (count == 0) continue;

        qsort(intersections, count, sizeof(intersection_t), compare_intersections);

        mfloat_t uv[VEC2_SIZE];
        mfloat_t st[VEC2_SIZE];
        mfloat_t p[VEC2_SIZE];
        mfloat_t r[VEC2_SIZE];

        // Draw all scanline segments
        for (int i = 0; i < count; i += 2) {
            float x0 = floorf(intersections[i].x);
            float x1 = floorf(intersections[i + 1].x);

            int j = intersections[i].index;

            // Vector from edge tail to tip
            mfloat_t* e = edge_vectors[j];

            // Vector from edge tail to scanline midpoint
            vec2(r, (x1 + x0) / 2.0f, y);
            vec2_subtract(r, points[j], r);

            bool flip = vec2_cross(e, r) > 0;

            // Draw scanline segment
            for (int x = x0; x <= x1; x++) {
                vec2(p, x, y);
                inverse_bilinear(uv, p, points[0], points[1], points[2], points[3], flip);
                vec2(st, uv[0] * graphics_texture_width_get(texture_map), uv[1] * graphics_texture_height_get(texture_map));
                vec2_floor(st, st);

                color_t color = graphics_texture_pixel_get(texture_map, st[0], st[1]);
                graphics_draw_pixel(destination, x, y, color);
            }
        }
    }
}

void graphics_draw_texture(texture_t* destination, texture_t* source, int x, int y, int width, int height) {
    rect_t dest_rect = {x, y, width, height};

    graphics_blit(
        source,
        destination,
        NULL,
        &dest_rect,
        draw_blit_func
    );
}

void graphics_draw_affine_texture(texture_t* destination, texture_t* source, mfloat_t* matrix) {
    /**
     * Rendering a texture with an affine transformation:
     *
     * 1. Transform bounding points of 0-1 UV space to screen space.
     *
     * 2. Determine bounds of transformed points.
     *
     * 3. Calculate first scanline UV coords by multipling by inverse matrix.
     *
     * 4. Calculate UV increment vector.
     *
     * 5. Render bounds as a series of scanlines.
     *
     *    a. Clip current scanline & UVs to screen.
     *
     *    b. Render clipped scanline using graphics_draw_textured_line.
     *
     *    c. Update current UV coords by UV increment vector.
     */

    // 1. Transform UV space bounds to screen space
    mfloat_t p0[VEC3_SIZE] = {0, 0, 1};
    mfloat_t p1[VEC3_SIZE] = {0, 1, 1};
    mfloat_t p2[VEC3_SIZE] = {1, 1, 1};
    mfloat_t p3[VEC3_SIZE] = {1, 0, 1};

    vec3_multiply_mat3(p0, p0, matrix);
    vec3_multiply_mat3(p1, p1, matrix);
    vec3_multiply_mat3(p2, p2, matrix);
    vec3_multiply_mat3(p3, p3, matrix);

    // 2. Determine screen space bounds
    mfloat_t min[VEC3_SIZE];
    mfloat_t max[VEC3_SIZE];
    vec3_assign(min, p0);
    vec3_assign(max, p0);

    vec3_min(min, min, p1);
    vec3_min(min, min, p2);
    vec3_min(min, min, p3);

    vec3_max(max, max, p1);
    vec3_max(max, max, p2);
    vec3_max(max, max, p3);

    vec3_floor(min, min);
    vec3_floor(max, max);

    // Get screen to UV transformation
    mfloat_t inverse[MAT3_SIZE];
    mat3_inverse(inverse, matrix);

    // 3. Calculate UV coordinates for first scanline
    mfloat_t uv0[VEC3_SIZE];
    mfloat_t uv1[VEC3_SIZE];
    vec3(uv0, min[0], min[1], 1);
    vec3(uv1, max[0] + 1, min[1], 1);
    vec3_multiply_mat3(uv0, uv0, inverse);
    vec3_multiply_mat3(uv1, uv1, inverse);

    // 4. Calculate UV increment vector
    mfloat_t inc[VEC3_SIZE];
    float one_pixel_vertical = 1.0f / graphics_texture_height_get(source);
    vec3(inc, 0, 1.0f + one_pixel_vertical, 0);
    vec3_multiply_mat3(inc, inc, inverse);

    int max_height = graphics_texture_height_get(destination);
    int max_width = graphics_texture_width_get(destination);

    // 5. Render scanlines
    for (int y = min[1]; y < max[1]; y++) {
        if (y >= max_height) break;

        if (y >= 0) {
            // Clip scanline to screen
            mfloat_t d[VEC3_SIZE];
            vec3_subtract(d, uv1, uv0);

            float x0 = min[0];
            float x1 = max[0];
            float u0 = uv0[0];
            float v0 = uv0[1];
            float u1 = uv1[0];
            float v1 = uv1[1];

            // 5a. Clip scanline

            // Clip left
            if (x0 < 0) {
                float t = x0 / (x1 - x0);
                x0 = 0;
                u0 = u0 - d[0] * t;
                v0 = v0 - d[1] * t;
            }

            // Clip right
            float w = max_width - 1;
            if (x1 > w) {
                float t = (x1 - w) / (x1 - x0);
                x1 = w;
                u1 = u1 - d[0] * t;
                v1 = v1 - d[1] * t;
            }

            // 5b. Draw scanline
            graphics_draw_textured_line(
                destination,
                x0, y,
                u0, v0,
                x1, y,
                u1, v1,
                source
            );
        }

        // 5c. Increment UV coords
        vec3_add(uv0, uv0, inc);
        vec3_add(uv1, uv1, inc);
    }
}

color_t* graphics_draw_palette_get(void) {
    return draw_palette;
}

void graphics_draw_palette_set(uint32_t* new_palette) {
    memmove(draw_palette, new_palette, sizeof(draw_palette));
}

void graphics_draw_palette_reset(void) {
    for (int i = 0; i < 256; i++) {
        draw_palette[i] = i;
    }
}

void graphics_draw_transparent_color_set(int color) {
    transparent_color = color;
}

int graphics_draw_transparent_color_get(void) {
    return transparent_color;
}

void graphics_draw_clipping_rectangle_set(rect_t* rect) {
    texture_t* render_texture = graphics_render_texture_get();

    rect_t default_rect = {
        0,
        0,
        graphics_texture_width_get(render_texture),
        graphics_texture_height_get(render_texture)
    };

    if (!rect) {
        rect = &default_rect;
    }

    clip_rect = *rect;
}

rect_t* graphics_draw_clipping_rectangle_get(void) {
    return &clip_rect;
}
