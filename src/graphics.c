#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "configuration.h"
#include "graphics.h"
#include "log.h"

static texture_t* render_texture = NULL;
static uint32_t palette[256];
static color_t draw_palette[256];
static int transparent_color = -1;

static rect_t clip_rect;

texture_t* graphics_texture_new(int width, int height, color_t* pixels) {
    texture_t* texture = (texture_t*)malloc(sizeof(texture_t));

    if (!texture) {
        log_error("Failed to create texture");
        return NULL;
    }

    texture->width = width;
    texture->height = height;
    texture->pixels = (color_t*)calloc(width * height, sizeof(color_t));

    if (texture->pixels == NULL) {
        log_fatal("Unable to allocate memory for texture pixels.");
    }

    if (pixels) {
        size_t size = width * height * sizeof(color_t);
        memmove(texture->pixels, pixels, size);
    }

    return texture;
}

void graphics_texture_free(texture_t* texture) {
    free(texture->pixels);
    texture->pixels = NULL;
    free(texture);
    texture = NULL;
}

texture_t* graphics_texture_copy(texture_t* texture) {
    return graphics_texture_new(texture->width, texture->height, texture->pixels);
}

void graphics_texture_clear(texture_t* texture, color_t color) {
    size_t number_of_bytes = texture->width * texture->height;
    memset(texture->pixels, color, number_of_bytes);
}

void graphics_texture_set_pixel(texture_t* texture, int x, int y, color_t color) {
    if (x < 0 || x >= texture->width) return;
    if (y < 0 || y >= texture->height) return;

    texture->pixels[y * texture->width + x] = color;
}

color_t graphics_texture_get_pixel(texture_t* texture, int x, int y) {
    if (x < 0 || x >= texture->width) return 0;
    if (y < 0 || y >= texture->height) return 0;

    return texture->pixels[y * texture->width + x];
}

typedef void(*pixel_copy_func_t)(texture_t* source_texture, texture_t* destination_texture, int sx, int sy, int dx, int dy);

static void blit(texture_t* source_texture, texture_t* destination_texture, rect_t* source_rect, rect_t* destination_rect, pixel_copy_func_t func) {
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
        s_left = abs(dx) * x_step;
    }

    if (dy < 0) {
        top = 0;
        s_top = abs(dy) * y_step;
    }

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

static void texture_blit_func(texture_t* source_texture, texture_t* destination_texture, int sx, int sy, int dx, int dy) {
    color_t pixel = graphics_texture_get_pixel(source_texture, sx, sy);
    graphics_texture_set_pixel(destination_texture, dx, dy, pixel);
}

void graphics_texture_blit(texture_t* source_texture, texture_t* destination_texture, rect_t* source_rect, rect_t* destination_rect) {
    blit(source_texture, destination_texture, source_rect, destination_rect, texture_blit_func);
}

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

texture_t* graphics_get_render_texture(void) {
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

void graphics_set_pixel(int x, int y, color_t color) {
    if (x < clip_rect.x || x >= clip_rect.x + clip_rect.width) return;
    if (y < clip_rect.y || y >= clip_rect.y + clip_rect.height) return;
    if (color == transparent_color) return;

    graphics_texture_set_pixel(render_texture, x, y, color);
}

static void graphics_blit_func(texture_t* source_texture, texture_t* _, int sx, int sy, int dx, int dy) {
    color_t pixel = graphics_texture_get_pixel(source_texture, sx, sy);
    pixel = draw_palette[pixel];

    graphics_set_pixel(dx, dy, pixel);
}

void graphics_blit(texture_t* texture, rect_t* source_rect, rect_t* destination_rect) {
    blit(texture, render_texture, source_rect, destination_rect, graphics_blit_func);
}

void graphics_set_clipping_rectangle(int x, int y, int width, int height) {
    clip_rect.x = x;
    clip_rect.y = y;
    clip_rect.width = width;
    clip_rect.height = height;
}
