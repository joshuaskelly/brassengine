#include <stdlib.h>

#include <mathc/mathc.h>

#include "mode7.h"

mode7_renderer_t* mode7_renderer_new(texture_t* render_texture) {
    mode7_renderer_t* renderer = (mode7_renderer_t*)malloc(sizeof(mode7_renderer_t));

    if (!render_texture) {
        renderer->render_texture = graphics_render_texture_get();
    }
    else {
        renderer->render_texture = render_texture;
    }

    mat3_identity(renderer->matrix);

    renderer->features.wrap_mode = WRAP_NONE;

    return renderer;
}

void mode7_renderer_free(mode7_renderer_t* renderer) {
    free(renderer);
    renderer = NULL;
}

void draw_row(mode7_renderer_t* renderer, int y, float u0, float v0, float u1, float v1, texture_t* texture);

void mode7_renderer_render(mode7_renderer_t* renderer, texture_t* texture, mode7_callback_t callback) {
    mfloat_t inverse[MAT3_SIZE];
    mat3_zero(inverse);

    mfloat_t uv0[VEC3_SIZE];
    mfloat_t uv1[VEC3_SIZE];
    mfloat_t work[VEC3_SIZE];
    vec3_zero(uv0);
    vec3_zero(uv1);
    vec3_zero(work);

    for (int y = 0; y < renderer->render_texture->height; y++) {
        if (callback(y)) {
            return;
        }

        mat3_inverse(inverse, renderer->matrix);

        vec3(work, 0, y, 1);
        vec3_multiply_mat3(uv0, work, inverse);

        vec3(work, renderer->render_texture->width - 1, y, 1);
        vec3_multiply_mat3(uv1, work, inverse);

        draw_row(
            renderer,
            y,
            uv0[0], uv0[1],
            uv1[0], uv1[1],
            texture
        );
    }
}

void draw_row(mode7_renderer_t* renderer, int y, float u0, float v0, float u1, float v1, texture_t* texture) {
    // DDA based line drawing algorithm
    texture_t* render_texture = renderer->render_texture;

    int x0 = 0;
    int x1 = render_texture->width - 1;

    int delta_x = x1 - x0;
    int delta_y = 0;
    int longest_side = fmax(abs(delta_x), abs(delta_y));

    float x_inc = delta_x / (float)longest_side;
    float y_inc = delta_y / (float)longest_side;

    float current_x = x0 + 0.5f;
    float current_y = y + 0.5f;

    float s0 = u0 * texture->width;
    float t0 = v0 * texture->height;
    float s1 = u1 * texture->width;
    float t1 = v1 * texture->height;

    float delta_s = s1 - s0;
    float delta_t = t1 - t0;
    float st_longest_side = fmax(fabs(delta_s), fabs(delta_t));

    float r = st_longest_side / longest_side;

    float s_inc = delta_s / st_longest_side * r;
    float t_inc = delta_t / st_longest_side * r;

    float current_s = s0 + 0.5f;
    float current_t = t0 + 0.5f;

    for (int i = 0; i <= longest_side; i++) {
        color_t c = graphics_texture_pixel_get(texture, current_s, current_t);

        graphics_texture_pixel_set(renderer->render_texture, current_x, current_y, c);

        current_x += x_inc;
        current_y += y_inc;
        current_s += s_inc;
        current_t += t_inc;
    }
}
