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

    rect_t default_source_rect = {0, 0, source_texture->width, source_texture->height};
    if (!source_rect) {
        source_rect = &default_source_rect;
    }

    rect_t default_destination_rect = {0, 0, destination_texture->width, destination_texture->height};
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

    if (bottom > destination_texture->height) {
        bottom = destination_texture->height;
    }

    if (right > destination_texture->width) {
        right = destination_texture->width;
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
        *width = render_texture->width;
    }

    if (height) {
        *height = render_texture->height;
    }
}

void graphics_clipping_rectangle_set(rect_t* rect) {
    rect_t default_rect = {0, 0, render_texture->width, render_texture->height};

    if (!rect) {
        rect = &default_rect;
    }

    clip_rect = *rect;
}

rect_t* graphics_clipping_rectangle_get(void) {
    return &clip_rect;
}


texture_t* graphics_texture_new(int width, int height, const color_t* pixels) {
    texture_t* texture = (texture_t*)malloc(sizeof(texture_t));

    if (!texture) {
        log_error("Failed to create texture");
        return NULL;
    }

    texture->pixels = (color_t*)malloc(width * height * sizeof(color_t));

    if (!texture->pixels) {
        log_error("Failed to create texture pixels");
        return NULL;
    }

    texture->width = width;
    texture->height = height;
    texture->stride = width;
    texture->is_subtexture = false;
    memset(texture->pixels, 0, width * height);

    if (pixels) {
        size_t size = width * height * sizeof(color_t);
        memmove(texture->pixels, pixels, size);
    }

    return texture;
}

void graphics_texture_free(texture_t* texture) {
    if (!texture->is_subtexture) {
        free(texture->pixels);
        texture->pixels = NULL;
    }

    free(texture);
    texture = NULL;
}

texture_t* graphics_texture_copy(texture_t* texture) {
    texture_t* copy = graphics_texture_new(
        texture->width,
        texture->height,
        NULL
    );

    size_t size = copy->width * sizeof(color_t);
    for (int i = 0; i < copy->height; i++) {
        memmove(
            copy->pixels + i * copy->stride,
            texture->pixels + i * texture->stride,
            size
        );
    }

    return copy;
}

void graphics_texture_clear(texture_t* texture, color_t color) {
    size_t size = texture->width * sizeof(color_t);

    // Call memset on a per row basis to ensure correct behavior
    // for sub textures.
    for (int i = 0; i < texture->height; i++) {
        memset(texture->pixels + i * texture->stride, color, size);
    }
}

texture_t* graphics_texture_sub(texture_t* texture, rect_t* rect) {
    if (
        rect->x < 0 ||
        rect->y < 0 ||
        rect->x + rect->width > texture->width ||
        rect->y + rect->height > texture->height
        ) {
        log_error("Subtexture rect outside source texture bounds");
        return NULL;
    }

    texture_t* sub_texture = (texture_t*)malloc(sizeof(texture_t));

    if (!sub_texture) {
        log_error("Failed to create texture");
        return NULL;
    }

    sub_texture->width = rect->width;
    sub_texture->height = rect->height;
    sub_texture->stride = texture->stride;
    sub_texture->is_subtexture = true;

    size_t offset = rect->x + rect->y * texture->stride;

    sub_texture->pixels = texture->pixels + offset;

    return sub_texture;
}

void graphics_texture_pixel_set(texture_t* texture, int x, int y, color_t color) {
    if (x < 0 || x >= texture->width) return;
    if (y < 0 || y >= texture->height) return;

    texture->pixels[y * texture->stride + x] = color;
}

color_t graphics_texture_pixel_get(texture_t* texture, int x, int y) {
    if (x < 0 || x >= texture->width) return transparent_color;
    if (y < 0 || y >= texture->height) return transparent_color;

    return texture->pixels[y * texture->stride + x];
}

static void texture_blit_func(texture_t* source_texture, texture_t* destination_texture, int sx, int sy, int dx, int dy) {
    color_t pixel = graphics_texture_pixel_get(source_texture, sx, sy);
    if (pixel == transparent_color) return;

    graphics_texture_pixel_set(destination_texture, dx, dy, pixel);
}

void graphics_texture_blit(texture_t* source_texture, texture_t* destination_texture, rect_t* source_rect, rect_t* destination_rect) {
    graphics_blit(source_texture, destination_texture, source_rect, destination_rect, texture_blit_func);
}
