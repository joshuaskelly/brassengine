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

void texture_blit(texture_t* source_texture, texture_t* destination_texture, rect_t* source_rect, rect_t* destination_rect) {
    rect_t s = {0, 0, source_texture->width, source_texture->height};
    if (!source_rect) {
        source_rect = &s;
    }

    rect_t d = {0, 0, destination_texture->width, destination_texture->height};
    if (!destination_rect) {
        destination_rect = &d;
    }

    float x_step = source_rect->width / (float)destination_rect->width;
    float y_step = source_rect->height / (float)destination_rect->height;

    float source_x = source_rect->x;
    float source_y = source_rect->y;

    for (int y = destination_rect->y; y < destination_rect->y + destination_rect->height; y++) {
        source_x = source_rect->x;

        for (int x = destination_rect->x; x < destination_rect->x + destination_rect->width; x++) {
            color_t pixel = texture_get_pixel(source_texture, source_x, source_y);
            texture_set_pixel(destination_texture, x, y, pixel);

            source_x += x_step;
        }

        source_y += y_step;
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
