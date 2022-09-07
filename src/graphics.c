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
    rect_t s;
    s.x = source_rect ? source_rect->x : 0;
    s.y = source_rect ? source_rect->y : 0;
    s.width = source_rect ? source_rect->width : source->width;
    s.height = source_rect ? source_rect->height : source->height;

    rect_t d;
    d.x = destination_rect ? destination_rect->x : 0;
    d.y = destination_rect ? destination_rect->y : 0;
    d.width = destination_rect ? destination_rect->width : destination->width;
    d.height = destination_rect ? destination_rect->height : destination->height;

    int horizontal_bound = fmin(destination->width, d.x + d.width);
    int vertical_bound = fmin(destination->height, d.y + d.height);

    for (int y = d.y; y < vertical_bound; y++) {
        float ry = (y - d.y) / (float)d.height;
        float iy = ry * s.height + s.y;

        for (int x = d.x; x < horizontal_bound; x++) {
            float rx = (x - d.x) / (float)d.width;
            float ix = rx * s.width + s.x;

            color_t pixel = texture_get_pixel(source, round(ix), round(iy));
            texture_set_pixel(destination, x, y, pixel);
        }
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
