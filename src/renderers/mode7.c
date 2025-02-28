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

void mode7_renderer_clear_color(mode7_renderer_t* renderer, color_t color) {
    graphics_texture_clear(renderer->render_texture, color);
}

void mode7_renderer_free(mode7_renderer_t* renderer) {
    free(renderer);
    renderer = NULL;
}

static void draw_scanline(mode7_renderer_t* renderer, int y, float u0, float v0, float u1, float v1, texture_t* texture);

void mode7_renderer_render(mode7_renderer_t* renderer, texture_t* texture, mode7_callback_t callback) {
    mfloat_t st0[VEC3_SIZE];
    mfloat_t st1[VEC3_SIZE];
    mfloat_t work[VEC3_SIZE];
    vec3_zero(st0);
    vec3_zero(st1);
    vec3_zero(work);

    for (int y = 0; y < renderer->render_texture->height; y++) {
        // Per scanline callback
        callback(y);

        // Transform scanline start
        vec3(work, 0, y, 1);
        vec3_multiply_mat3(st0, work, renderer->matrix);

        // Transform scanline end
        vec3(work, renderer->render_texture->width - 1, y, 1);
        vec3_multiply_mat3(st1, work, renderer->matrix);

        draw_scanline(
            renderer,
            y,
            st0[0], st0[1],
            st1[0], st1[1],
            texture
        );
    }
}

static void draw_scanline(mode7_renderer_t* renderer, int y, float s0, float t0, float s1, float t1, texture_t* texture) {
    // DDA based line drawing algorithm
    texture_t* render_texture = renderer->render_texture;

    int scanline_width = render_texture->width - 1;

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

        if (c != graphics_transparent_color_get()) {
            graphics_texture_pixel_set(renderer->render_texture, x, y, c);
        }

        current_s += s_inc;
        current_t += t_inc;
    }
}

mode7_camera_t* mode7_camera_new(mode7_renderer_t* renderer) {
    mode7_camera_t* camera = (mode7_camera_t*)malloc(sizeof(mode7_camera_t));

    vec3_zero(camera->position);

    camera->fov = 72.0f;
    camera->near = 0.0f;
    camera->far = 10000.0f;

    camera->renderer = renderer;

    return camera;
}

void mode7_camera_free(mode7_camera_t* camera) {
    camera->renderer = NULL;
    free(camera);
    camera = NULL;
}

void mode7_camera_call(mode7_camera_t* camera, int scanline) {
    mode7_renderer_t* renderer = camera->renderer;
    texture_t* rt = renderer->render_texture;

    float top = rt->height / 2.0f;
    float left = -rt->width / 2.0f;

    mfloat_t m[MAT3_SIZE];
    mat3_zero(m);

    float D = (rt->width / 2.0f) / tanf(camera->fov / 2.0f * MPI / 180.0f);

    float wy = sinf(camera->pitch * MPI / 180.0f);
    float vy = cosf(camera->pitch * MPI / 180.0f);

    // Calculate horizon location
    int horizon = 0;
    if (vy != 0) {
        horizon = camera->far * wy - camera->position[1];
        horizon = top - ((horizon * D) / (camera->far * vy));
    }
    else {
        horizon = wy > 0 ? INT_MIN : INT_MAX;
    }

    // Early out if horizon below screen
    if (horizon > rt->height) {
        mat3_assign(camera->renderer->matrix, m);
        return;
    }

    // Early out if scanline above horizon
    if (scanline < horizon) {
        mat3_assign(camera->renderer->matrix, m);
        return;
    }

    float xc = camera->position[0];
    float yc = camera->position[1];
    float zc = camera->position[2];

    float cf = cosf(camera->yaw * MPI / 180.0f);
    float sf = sinf(camera->yaw * MPI / 180.0f);
    float ct = cosf(camera->pitch * MPI / 180.0f);
    float st = sinf(camera->pitch * MPI / 180.0f);

    float yb = (scanline - top) * ct + D * st;
    float lam = yc / yb;

    float lcf = lam * cf;
    float lsf = lam * sf;

    float a = lcf;
    float c = lsf;

    float zb = (scanline - top) * st - D * ct;

    float x = xc + lcf * left - lsf * zb;
    float y = zc + lsf * left + lcf * zb;

    mfloat_t basis[MAT3_SIZE];
    mat3_identity(basis);
    basis[0] = a;
    basis[1] = c;
    basis[3] = 0.0f;
    basis[4] = 0.0f;

    mfloat_t translation[MAT3_SIZE];
    mat3_identity(translation);
    translation[6] = x;
    translation[7] = y;

    mat3_multiply(m, translation, basis);

    mat3_assign(camera->renderer->matrix, m);
}
