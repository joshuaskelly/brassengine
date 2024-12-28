#ifndef RENDERERS_MODE7_H
#define RENDERERS_MODE7_H

#include <stdbool.h>

#include <mathc/mathc.h>

#include "../graphics.h"

typedef enum {
    WRAP_NONE = 0,
    WRAP_REPEAT,
    WRAP_CLAMP
} mode7_wrap_mode_t;

typedef struct {
    texture_t* render_texture;
    mfloat_t matrix[MAT3_SIZE];

    struct {
        mode7_wrap_mode_t wrap_mode;
    } features;
} mode7_renderer_t;

typedef void(*mode7_callback_t)(int);

mode7_renderer_t* mode7_renderer_new(texture_t* render_texture);

void mode7_renderer_clear_color(mode7_renderer_t* renderer, color_t color);

void mode7_renderer_free(mode7_renderer_t* renderer);

void mode7_renderer_render(mode7_renderer_t* renderer, texture_t* texture, mode7_callback_t callback);

#endif
