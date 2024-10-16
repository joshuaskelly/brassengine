#include <stdlib.h>

#include <mathc/mathc.h>

#include "../math.h"

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

void draw_scanline(mode7_renderer_t* renderer, int y, float u0, float v0, float u1, float v1, texture_t* texture);

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

        draw_scanline(
            renderer,
            y,
            uv0[0], uv0[1],
            uv1[0], uv1[1],
            texture
        );
    }
}

void draw_scanline(mode7_renderer_t* renderer, int y, float u0, float v0, float u1, float v1, texture_t* texture) {
    // DDA based line drawing algorithm
    texture_t* render_texture = renderer->render_texture;

    int scanline_width = render_texture->width - 1;

    float s0 = u0 * (texture->width - 1);
    float t0 = v0 * (texture->height - 1);
    float s1 = u1 * (texture->width - 1);
    float t1 = v1 * (texture->height - 1);

    float delta_s = s1 - s0;
    float delta_t = t1 - t0;

    float s_inc = delta_s / scanline_width;
    float t_inc = delta_t / scanline_width;

    float current_s = s0 + 0.5f;
    float current_t = t0 + 0.5f;

    for (int x = 0; x <= scanline_width; x++) {
        float s = current_s;
        float t = current_t;

        if (renderer->features.wrap_mode == WRAP_REPEAT) {
            s = modulof(s, texture->width - 1);
            t = modulof(t, texture->height - 1);
        }
        else if (renderer->features.wrap_mode == WRAP_CLAMP) {
            s = clamp(s, 0, texture->width - 1);
            t = clamp(t, 0, texture->height - 1);
        }

        color_t c = graphics_texture_pixel_get(texture, s, t);

        graphics_texture_pixel_set(renderer->render_texture, x, y, c);

        current_s += s_inc;
        current_t += t_inc;
    }
}
