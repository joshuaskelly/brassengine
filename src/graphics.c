#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "configuration.h"
#include "event.h"
#include "graphics.h"
#include "log.h"

static texture_t* render_texture = NULL;
static uint32_t palette[256];
static color_t draw_palette[256];
static color_t transparent_color = 0;

static rect_t clip_rect;

void graphics_init(void) {
    log_info("graphics init");

    render_texture = graphics_texture_new(
        config->resolution.width,
        config->resolution.height,
        NULL
    );

    if (!render_texture) {
        log_fatal("Failed to create frame buffer");
    }

    clip_rect.x = 0;
    clip_rect.y = 0;
    clip_rect.width = config->resolution.width;
    clip_rect.height = config->resolution.height;

    for (int i = 0; i < 256; i++) {
        draw_palette[i] = i;
    }
}

void graphics_destroy(void) {
    graphics_texture_free(render_texture);
}

void graphics_reload(void) {
    for (int i = 0; i < 256; i++) {
        draw_palette[i] = i;
    }
}

texture_t* graphics_render_texture_get(void) {
    return render_texture;
}

uint32_t* graphics_palette_get(void) {
    return palette;
}

void graphics_palette_set(uint32_t* new_palette) {
    memmove(palette, new_palette, sizeof(palette));
}

void graphics_palette_clear(void) {
    memset(palette, 0, sizeof(palette));
}

color_t* graphics_draw_palette_get(void) {
    return draw_palette;
}

void graphics_draw_palette_set(uint32_t* new_palette) {
    memmove(draw_palette, new_palette, sizeof(draw_palette));
}

void graphics_draw_palette_clear(void) {
    memset(draw_palette, 0, sizeof(draw_palette));
}

void graphics_transparent_color_set(int color) {
    transparent_color = color;
}

int graphics_transparent_color_get(void) {
    return transparent_color;
}

void graphics_pixel_set(int x, int y, color_t color) {
    if (x < clip_rect.x || x >= clip_rect.x + clip_rect.width) return;
    if (y < clip_rect.y || y >= clip_rect.y + clip_rect.height) return;
    if (color == transparent_color) return;

    graphics_texture_pixel_set(render_texture, x, y, color);
}

static void default_blit_func(texture_t* source_texture, texture_t* _, int sx, int sy, int dx, int dy) {
    color_t pixel = graphics_texture_pixel_get(source_texture, sx, sy);
    pixel = draw_palette[pixel];

    graphics_pixel_set(dx, dy, pixel);
}

void graphics_blit(texture_t* source_texture, texture_t* destination_texture, rect_t* source_rect, rect_t* destination_rect, pixel_copy_func_t func) {
    if (!destination_texture) {
        destination_texture = render_texture;
    }

    if (!func) {
        func = default_blit_func;
    }

    rect_t default_source_rect = {
        0,
        0,
        graphics_texture_width_get(source_texture),
        graphics_texture_height_get(source_texture)
    };
    if (!source_rect) {
        source_rect = &default_source_rect;
    }

    rect_t default_destination_rect = {
        0,
        0,
        graphics_texture_width_get(destination_texture),
        graphics_texture_height_get(destination_texture)
    };
    if (!destination_rect) {
        destination_rect = &default_destination_rect;
    }

    float x_step = source_rect->width / (float)destination_rect->width;
    float y_step = source_rect->height / (float)destination_rect->height;

    int left = destination_rect->x;
    int right = destination_rect->x + destination_rect->width;
    int top = destination_rect->y;
    int bottom = destination_rect->y + destination_rect->height;

    int dy = top;
    int dx = left;
    float sx = source_rect->x;
    float sy = source_rect->y;
    float s_left = source_rect->x;
    float s_top = source_rect->y;

    // Adjust draw boundaries to destination texture
    if (dx < 0) {
        left = 0;
        s_left += abs(dx) * x_step;
    }

    if (dy < 0) {
        top = 0;
        s_top += abs(dy) * y_step;
    }

    // Sample source at pixel centers
    s_top += 0.5f * y_step;
    s_left += 0.5f * x_step;

    if (bottom > graphics_texture_height_get(destination_texture)) {
        bottom = graphics_texture_height_get(destination_texture);
    }

    if (right > graphics_texture_width_get(destination_texture)) {
        right = graphics_texture_width_get(destination_texture);
    }

    for (dy = top, sy = s_top; dy < bottom; dy++, sy += y_step) {
        for (dx = left, sx = s_left; dx < right; dx++, sx += x_step) {
            func(source_texture, destination_texture, sx, sy, dx, dy);
        }
    }
}

void graphics_resolution_set(int width, int height) {
    graphics_texture_free(render_texture);
    render_texture = graphics_texture_new(width, height, NULL);

    if (!render_texture) {
        log_fatal("Failed to create frame buffer");
    }

    // Post event on successfully changing resolution
    event_t event;
    event.type = EVENT_GRAPHICSRESOLUTIONCHANGED;
    event.graphics_resolution_change.width = width;
    event.graphics_resolution_change.height = height;
    event_post(&event);

    clip_rect.x = 0;
    clip_rect.y = 0;
    clip_rect.width = width;
    clip_rect.height = height;
}

void graphics_resolution_get(int* width, int* height) {
    if (width) {
        *width = graphics_texture_width_get(render_texture);
    }

    if (height) {
        *height = graphics_texture_height_get(render_texture);
    }
}

void graphics_clipping_rectangle_set(rect_t* rect) {
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

rect_t* graphics_clipping_rectangle_get(void) {
    return &clip_rect;
}
