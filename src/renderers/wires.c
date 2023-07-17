#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <mathc/mathc.h>

#include "../graphics.h"
#include "../collections/float_array.h"
#include "../collections/int_array.h"
#include "../renderers/draw.h"

#include "wires.h"

typedef struct {
    mfloat_t a[VEC4_SIZE];
    mfloat_t b[VEC4_SIZE];
    color_t color;
} line_t;

static bool clip(wires_renderer_t* renderer, line_t* line);
static bool clip_line_plane(line_t* line, mfloat_t* point, mfloat_t* normal);
static void wires_texture_set_depth_pixel(wires_renderer_t* renderer, int x, int y, float w, color_t color);
static void wires_draw_line(wires_renderer_t* renderer, int x0, int y0, float w0, int x1, int y1, float w1, color_t color);

wires_line_buffer_t* wires_line_buffer_new(int line_count, int vertex_count) {
    wires_line_buffer_t* line_buffer = (wires_line_buffer_t*)malloc(sizeof(wires_line_buffer_t));

    line_buffer->indices = int_array_new(line_count * 2);
    line_buffer->colors = int_array_new(line_count);
    line_buffer->vertices = float_array_new(VEC3_SIZE * vertex_count);

    return line_buffer;
}

void wires_line_buffer_free(wires_line_buffer_t* lines) {
    float_array_free(lines->vertices);
    lines->vertices = NULL;
    int_array_free(lines->colors);
    lines->colors = NULL;
    int_array_free(lines->indices);
    lines->indices = NULL;
    free(lines);
    lines = NULL;
}

wires_renderer_t* wires_renderer_new(void) {
    wires_renderer_t* renderer = (wires_renderer_t*)malloc(sizeof(wires_renderer_t));

    renderer->clip_planes.near.point[0] = 0.0f;
    renderer->clip_planes.near.point[1] = 0.0f;
    renderer->clip_planes.near.point[2] = 0.0f;
    renderer->clip_planes.near.normal[0] = 0.0f;
    renderer->clip_planes.near.normal[1] = 0.0f;
    renderer->clip_planes.near.normal[2] = 1.0f;

    renderer->clip_planes.far.point[0] = 0.0f;
    renderer->clip_planes.far.point[1] = 0.0f;
    renderer->clip_planes.far.point[2] = 0.0f;
    renderer->clip_planes.far.normal[0] = 0.0f;
    renderer->clip_planes.far.normal[1] = 0.0f;
    renderer->clip_planes.far.normal[2] = -1.0f;

    renderer->clip_planes.left.point[0] = 0.0f;
    renderer->clip_planes.left.point[1] = 0.0f;
    renderer->clip_planes.left.point[2] = 0.0f;
    renderer->clip_planes.left.normal[0] = 0.0f;
    renderer->clip_planes.left.normal[1] = 0.0f;
    renderer->clip_planes.left.normal[2] = 0.0f;

    renderer->clip_planes.right.point[0] = 0.0f;
    renderer->clip_planes.right.point[1] = 0.0f;
    renderer->clip_planes.right.point[2] = 0.0f;
    renderer->clip_planes.right.normal[0] = 0.0f;
    renderer->clip_planes.right.normal[1] = 0.0f;
    renderer->clip_planes.right.normal[2] = 0.0f;

    renderer->clip_planes.top.point[0] = 0.0f;
    renderer->clip_planes.top.point[1] = 0.0f;
    renderer->clip_planes.top.point[2] = 0.0f;
    renderer->clip_planes.top.normal[0] = 0.0f;
    renderer->clip_planes.top.normal[1] = 0.0f;
    renderer->clip_planes.top.normal[2] = 0.0f;

    renderer->clip_planes.bottom.point[0] = 0.0f;
    renderer->clip_planes.bottom.point[1] = 0.0f;
    renderer->clip_planes.bottom.point[2] = 0.0f;
    renderer->clip_planes.bottom.normal[0] = 0.0f;
    renderer->clip_planes.bottom.normal[1] = 0.0f;
    renderer->clip_planes.bottom.normal[2] = 0.0f;

    texture_t* render_texture = graphics_get_render_texture();
    size_t size = render_texture->width * render_texture->height;

    renderer->depth_buffer = (float*)malloc(sizeof(float) * size);
    memset(renderer->depth_buffer, 0, sizeof(float) * size);

    return renderer;
}

void wires_renderer_free(wires_renderer_t* renderer) {
    free(renderer->depth_buffer);
    renderer->depth_buffer = NULL;
    free(renderer);
    renderer = NULL;
}

void wires_renderer_start(wires_renderer_t* renderer) {
    renderer->statistics.lines_rendered = 0;
}

void wires_renderer_stop(wires_renderer_t* renderer) {

}

void wires_renderer_clear_color(wires_renderer_t* renderer, color_t color) {
    texture_t* render_texture = graphics_get_render_texture();
    graphics_texture_clear(render_texture, color);
}

void wires_renderer_clear_depth(wires_renderer_t* renderer, float depth) {
    texture_t* render_texture = graphics_get_render_texture();
    size_t size = render_texture->width * render_texture->height;

    for (int i = 0; i < size; i++) {
        renderer->depth_buffer[i] = depth;
    }
}

void wires_renderer_render(wires_renderer_t* renderer, mfloat_t* model_matrix, wires_line_buffer_t* lines) {
    texture_t* render_texture = graphics_get_render_texture();
    float width = render_texture->width;
    float height = render_texture->height;

    mfloat_t model_view_matrix[MAT4_SIZE];
    mat4_multiply(model_view_matrix, renderer->view_matrix, model_matrix);

    int line_count = lines->indices->size / 2;

    for (int i = 0; i < line_count; i++) {
        line_t line;

        mfloat_t* a = &lines->vertices->data[lines->indices->data[(i * 2 + 0)] * VEC3_SIZE];
        mfloat_t* b = &lines->vertices->data[lines->indices->data[(i * 2 + 1)] * VEC3_SIZE];

        vec3_assign(line.a, a);
        line.a[3] = 1.0f;

        vec3_assign(line.b, b);
        line.b[3] = 1.0f;

        line.color = lines->colors->data[i];

        // Model view transformation
        vec4_multiply_mat4(line.a, line.a, model_view_matrix);
        vec4_multiply_mat4(line.b, line.b, model_view_matrix);

        if (!clip(renderer, &line)) continue;

        // Projection
        vec4_multiply_mat4(line.a, line.a, renderer->projection_matrix);
        vec4_multiply_mat4(line.b, line.b, renderer->projection_matrix);

        // Perspective divide
        float w = line.a[3];
        vec4_divide_f(line.a, line.a, w);
        line.a[3] = w;

        w = line.b[3];
        vec4_divide_f(line.b, line.b, w);
        line.b[3] = w;

        line.a[0] = (line.a[0] + 1) * 0.5 * (width - 1);
        line.a[1] = (line.a[1] + 1) * 0.5 * (height - 1);
        line.b[0] = (line.b[0] + 1) * 0.5 * (width - 1);
        line.b[1] = (line.b[1] + 1) * 0.5 * (height - 1);

        wires_draw_line(
            renderer,
            line.a[0],
            line.a[1],
            line.a[3],
            line.b[0],
            line.b[1],
            line.b[3],
            line.color
        );

        renderer->statistics.lines_rendered++;
    }
}

void wires_renderer_camera(wires_renderer_t* renderer, float near, float far, float fov, mfloat_t view_matrix[MAT4_SIZE]) {
    texture_t* render_texture = graphics_get_render_texture();
    float width = render_texture->width;
    float height = render_texture->height;
    float aspect = width / height;

    mat4_assign(renderer->view_matrix, view_matrix);
    mat4_perspective_fov(
        renderer->projection_matrix,
        fov,
        width,
        height,
        near,
        far
    );

    float fov_horizontal = atan(tan(fov / 2.0f) * aspect) * 2.0f;
    float chfh = cosf(fov_horizontal / 2.0f);
    float shfh = sinf(fov_horizontal / 2.0f);

    float chfv = cosf(fov / 2.0f);
    float shfv = sinf(fov / 2.0f);

    renderer->clip_planes.near.point[2] = near;
    renderer->clip_planes.far.point[2] = far;
    renderer->clip_planes.left.normal[0] = chfh;
    renderer->clip_planes.left.normal[2] = shfh;
    renderer->clip_planes.right.normal[0] = -chfh;
    renderer->clip_planes.right.normal[2] = shfh;
    renderer->clip_planes.top.normal[1] = chfv;
    renderer->clip_planes.top.normal[2] = shfv;
    renderer->clip_planes.bottom.normal[1] = -chfv;
    renderer->clip_planes.bottom.normal[2] = shfv;
}

static bool clip(wires_renderer_t* renderer, line_t* line) {
    return clip_line_plane(line, renderer->clip_planes.near.point, renderer->clip_planes.near.normal) &&
        clip_line_plane(line, renderer->clip_planes.far.point, renderer->clip_planes.far.normal) &&
        clip_line_plane(line, renderer->clip_planes.left.point, renderer->clip_planes.left.normal) &&
        clip_line_plane(line, renderer->clip_planes.right.point, renderer->clip_planes.right.normal) &&
        clip_line_plane(line, renderer->clip_planes.top.point, renderer->clip_planes.top.normal) &&
        clip_line_plane(line, renderer->clip_planes.bottom.point, renderer->clip_planes.bottom.normal);
}

// https://fabiensanglard.net/polygon_codec/clippingdocument/Clipping.pdf
static bool clip_line_plane(line_t* line, mfloat_t* point, mfloat_t* normal) {
    mfloat_t q1[VEC3_SIZE];
    vec3_assign(q1, line->a);
    mfloat_t q2[VEC3_SIZE];
    vec3_assign(q2, line->b);

    mfloat_t r[VEC3_SIZE];
    float d1 = vec3_dot(vec3_subtract(r, q1, point), normal);
    float d2 = vec3_dot(vec3_subtract(r, q2, point), normal);

    if (d1 >= 0 && d2 >= 0) {
        return true;
    }

    if (d1 <= 0 && d2 <= 0) {
        return false;
    }

    if (d1 > 0 && d2 < 0) {
        float f = d1 / (d1 - d2);
        vec4_lerp(line->b, line->a, line->b, f);
        return true;
    }

    if (d1 < 0 && d2 > 0) {
        float f = d1 / (d1 - d2);
        vec4_lerp(line->a, line->a, line->b, f);
        return true;
    }

    return true;
}

static void wires_texture_set_depth_pixel(wires_renderer_t* renderer, int x, int y, float w, color_t color) {
    texture_t* texture = graphics_get_render_texture();
    if (x < 0 || x >= texture->width) return;
    if (y < 0 || y >= texture->height) return;

    float far = renderer->clip_planes.far.point[2];
    float depth = -w / far;

    if (renderer->depth_buffer[y * texture->width + x] < depth) return;

    texture->pixels[y * texture->width + x] = color;
    renderer->depth_buffer[y * texture->width + x] = depth;
}

static void wires_draw_line(wires_renderer_t* renderer, int x0, int y0, float w0, int x1, int y1, float w1, color_t color) {
    // DDA based line drawing algorithm
    int delta_x = x1 - x0;
    int delta_y = y1 - y0;
    int delta_w = w1 - w0;
    int longest_side = fmax(abs(delta_x), abs(delta_y));

    float x_inc = delta_x / (float)longest_side;
    float y_inc = delta_y / (float)longest_side;
    float w_inc = delta_w / (float)longest_side;

    float current_x = x0;
    float current_y = y0;
    float current_w = w0;

    for (int i = 0; i <= longest_side; i++) {
        wires_texture_set_depth_pixel(renderer, current_x, current_y, current_w, color);
        current_x += x_inc;
        current_y += y_inc;
        current_w += w_inc;
    }
}
