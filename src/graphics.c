#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "graphics.h"
#include "log.h"

static texture_t* render_texture = NULL;
static uint32_t* palette = NULL;

static struct {
    int x;
    int y;
    int width;
    int height;
} clip_rect;

texture_t* texture_new(int width, int height, color_t* pixels) {
    texture_t* texture = (texture_t*)malloc(sizeof(texture_t));

    if (!texture) {
        log_error("Failed to create texture");
        return NULL;
    }

    texture->width = width;
    texture->height = height;
    texture->pixels = (color_t*)calloc(width * height, sizeof(color_t));

    if (pixels) {
        size_t size = width * height * sizeof(color_t);
        memmove(texture->pixels, pixels, size);
    }

    return texture;
}

void texture_free(texture_t* texture) {
    free(texture->pixels);
    free(texture);
}

void texture_clear(texture_t* texture, color_t color) {
    size_t number_of_bytes = texture->width * texture->height;
    memset(texture->pixels, color, number_of_bytes);
}

void texture_set_pixel(texture_t* texture, int x, int y, color_t color) {
    if (x < 0 || x >= texture->width) return;
    if (y < 0 || y >= texture->height) return;

    texture->pixels[y * texture->width + x] = color;
}

color_t texture_get_pixel(texture_t* texture, int x, int y) {
    return texture->pixels[y * texture->width + x];
}

void texture_blit(texture_t* source, texture_t* destination, rect_t* source_rect, rect_t* destination_rect) {
    rect_t s = {0, 0, source->width, source->height};
    if (source_rect) {
        s = (rect_t){source_rect->x, source_rect->y, source_rect->width, source_rect->height};
    }

    rect_t d = {0, 0, destination->width, destination->height};
    if (destination_rect) {
        d = (rect_t){destination_rect->x, destination_rect->y, destination_rect->width, destination_rect->height};
    }

    float x_step = s.width / (float)d.width;
    float y_step = s.height / (float)d.height;

    float ix = 0;
    float iy = 0;

    for (int y = 0; y < d.height; y++) {

        ix = 0;
        for (int x = 0; x < d.width; x++) {
            color_t pixel = texture_get_pixel(source, ix + s.x, iy + s.y);
            texture_set_pixel(destination, x + d.x, y + d.y, pixel);

            ix += x_step;
        }

        iy += y_step;
    }
}

void graphics_init(void) {
    log_info("graphics init");

    render_texture = texture_new(
        RENDER_BUFFER_WIDTH,
        RENDER_BUFFER_HEIGHT,
        NULL
    );

    if (!render_texture) {
        log_fatal("Failed to create frame buffer");
    }

    palette = (uint32_t*)calloc(256, sizeof(uint32_t));

    if (!palette) {
        log_fatal("Failed to create palette");
    }

    palette[0] = 0xFF0000FF;
    palette[1] = 0xFFFFFFFF;

    clip_rect.x = 0;
    clip_rect.y = 0;
    clip_rect.width = RENDER_BUFFER_WIDTH;
    clip_rect.height = RENDER_BUFFER_HEIGHT;
}

void graphics_destroy(void) {
    free(palette);
    texture_free(render_texture);
}

texture_t* graphics_get_render_texture(void) {
    return render_texture;
}

uint32_t* graphics_get_palette(void) {
    return palette;
}

void graphics_set_pixel(int x, int y, color_t color) {
    if (x < clip_rect.x || x >= clip_rect.x + clip_rect.width) return;
    if (y < clip_rect.y || y >= clip_rect.y + clip_rect.height) return;

    texture_set_pixel(render_texture, x, y, color);
}

void graphics_set_clipping_rectangle(int x, int y, int width, int height) {
    clip_rect.x = x;
    clip_rect.y = y;
    clip_rect.width = width;
    clip_rect.height = height;
}
