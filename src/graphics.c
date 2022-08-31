#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "graphics.h"
#include "log.h"

texture_t* texture_new(int width, int height, color_t* pixels) {
    texture_t* texture = (texture_t*)malloc(sizeof(texture_t));

    if (!texture) {
        log_error("failed to create texture");
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
    if (x < 0 || x > texture->width) return;
    if (y < 0 || y > texture->height) return;

    texture->pixels[y * texture->width + x] = color;
}

color_t texture_get_pixel(texture_t* texture, int x, int y) {
    return texture->pixels[y * texture->width + x];
}

static texture_t* render_texture = NULL;
static uint32_t* palette = NULL;

void graphics_init(void) {
    render_texture = texture_new(
        RENDER_BUFFER_WIDTH,
        RENDER_BUFFER_HEIGHT,
        NULL
    );

    if (!render_texture) {
        log_fatal("Failed to create frame buffer.\n");
    }

    palette = (uint32_t*)calloc(256, sizeof(uint32_t));

    if (!palette) {
        log_fatal("Failed to create palette.\n");
    }

    palette[0] = 0xFF0000FF;
    palette[1] = 0xFFFFFFFF;
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
