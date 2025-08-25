#include <limits.h>
#include <stdlib.h>

#include <mathc/mathc.h>

#include "../graphics.h"
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
        vec3(work, 0.5f, y + 0.5f, 1);
        vec3_multiply_mat3(st0, work, renderer->matrix);

        // Transform scanline end
        vec3(work, renderer->render_texture->width + 0.5f, y + 0.5f, 1);
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

    color_t* draw_palette = graphics_draw_palette_get();

    int scanline_width = render_texture->width;

    float delta_s = s1 - s0;
    float delta_t = t1 - t0;

    float s_inc = delta_s / scanline_width;
    float t_inc = delta_t / scanline_width;

    float current_s = s0;
    float current_t = t0;

    for (int x = 0; x <= scanline_width; x++) {
        float s = current_s;
        float t = current_t;

        if (renderer->features.wrap_mode == WRAP_REPEAT) {
            s = modulof(s, texture->width);
            t = modulof(t, texture->height);
        }
        else if (renderer->features.wrap_mode == WRAP_CLAMP) {
            s = clamp(s, 0, texture->width - 1);
            t = clamp(t, 0, texture->height - 1);
        }
        else if (renderer->features.wrap_mode == WRAP_NONE) {
            // Avoid rounding up to zero
            if (s < 0 && s > -1.0f) s = -1.0f;
            if (t < 0 && t > -1.0f) t = -1.0f;
        }

        color_t c = graphics_texture_pixel_get(texture, s, t);
        c = draw_palette[c];

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
    /**
     * Perspective camera implementation.
     *
     * Adapated from: https://www.coranac.com/tonc/text/mode7ex.htm
     */

    mode7_renderer_t* renderer = camera->renderer;
    texture_t* rt = renderer->render_texture;

    float top = rt->height / 2.0f;
    float left = -rt->width / 2.0f;

    mfloat_t m[MAT3_SIZE];
    mat3_zero(m);

    float distance_to_projection_plane = (rt->width / 2.0f) / tanf(to_radians(camera->fov) / 2.0f);

    float yaw_radians = to_radians(camera->yaw);
    float pitch_radians = to_radians(camera->pitch);

    float cos_yaw = cosf(yaw_radians);
    float sin_yaw = sinf(yaw_radians);
    float cos_pitch = cosf(pitch_radians);
    float sin_pitch = sinf(pitch_radians);

    float camera_x = camera->position[0];
    float camera_y = camera->position[1];
    float camera_z = camera->position[2];

    // Calculate horizon location
    int horizon = 0;
    if (cos_yaw != 0) {
        horizon = camera->far * sin_pitch - camera_y;
        horizon = top - (horizon * distance_to_projection_plane) / (camera->far * cos_pitch);
    }
    else {
        horizon = sin_pitch > 0 ? INT_MIN : INT_MAX;
    }

    // Early out if horizon below screen
    if (horizon > rt->height) {
        mat3_assign(camera->renderer->matrix, m);
        return;
    }

    // Early out if scanline above horizon
    if (scanline <= horizon) {
        mat3_assign(camera->renderer->matrix, m);
        return;
    }

    float yb = (scanline - top) * cos_pitch + distance_to_projection_plane * sin_pitch;
    float scale = camera_y / yb;

    float scy = scale * cos_yaw;
    float ssy = scale * sin_yaw;

    float forward = (scanline - top) * sin_pitch - distance_to_projection_plane * cos_pitch;

    float x = camera_x - 0.5f + scy * left - ssy * forward;
    float y = camera_z - 0.5f + ssy * left + scy * forward;

    mfloat_t basis[MAT3_SIZE];
    mat3_identity(basis);
    basis[0] = scy;
    basis[1] = ssy;
    basis[3] = 0.0f;
    basis[4] = 0.0f;

    mfloat_t translation[MAT3_SIZE];
    mat3_identity(translation);
    translation[6] = x;
    translation[7] = y;

    mat3_multiply(m, translation, basis);

    mat3_assign(camera->renderer->matrix, m);
}
