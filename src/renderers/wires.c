#include <stdbool.h>

#include <mathc/mathc.h>

#include "../graphics.h"
#include "../renderers/draw.h"

#include "wires.h"

typedef struct {
    mfloat_t a[VEC4_SIZE];
    mfloat_t b[VEC4_SIZE];
    color_t color;
} line_t;

static bool clip(wires_renderer_t* renderer, line_t* line);
static bool clip_line_plane(line_t* line, mfloat_t* point, mfloat_t* normal);

void wires_renderer_start(wires_renderer_t* renderer) {
    renderer->statistics.lines_rendered = 0;
}

void wires_renderer_stop(wires_renderer_t* renderer) {

}

void wires_renderer_render(wires_renderer_t* renderer, mfloat_t* model_matrix, wires_line_buffer_t* lines) {
    texture_t* render_texture = graphics_get_render_texture();
    float width = render_texture->width;
    float height = render_texture->height;

    mfloat_t model_view_matrix[MAT4_SIZE];
    mat4_multiply(model_view_matrix, renderer->view_matrix, model_matrix);

    for (int i = 0; i < lines->line_count; i += 2) {
        line_t line;

        mfloat_t* a = lines->vertices[lines->indices[i + 0]];
        mfloat_t* b = lines->vertices[lines->indices[i + 1]];

        vec3_assign(line.a, a);
        line.a[3] = 1.0f;

        vec3_assign(line.b, b);
        line.b[3] = 1.0f;

        line.color = lines->colors[i / 2];

        vec4_multiply_mat4(line.a, line.a, model_view_matrix);
        vec4_multiply_mat4(line.b, line.b, model_view_matrix);

        if (!clip(renderer, &line)) continue;

        vec4_multiply_mat4(line.a, line.a, renderer->projection_matrix);
        vec4_multiply_mat4(line.b, line.b, renderer->projection_matrix);

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

        draw_line(line.a[0], line.a[1], line.b[0], line.b[1], line.color);

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
