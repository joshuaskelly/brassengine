#include <stdlib.h>
#include <string.h>

#include "texture.h"
#include "../graphics.h"
#include "../log.h"

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

size_t graphics_texture_sizeof(texture_t* texture) {
    return sizeof(texture_t) + texture->width * texture->height * sizeof(color_t);;
}

color_t* graphics_texture_pixels_get(texture_t* texture) {
    return texture->pixels;
}

int graphics_texture_width_get(texture_t* texture) {
    return texture->width;
}

int graphics_texture_height_get(texture_t* texture) {
    return texture->height;
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
    if (x < 0 || x >= texture->width) return graphics_transparent_color_get();
    if (y < 0 || y >= texture->height) return graphics_transparent_color_get();

    return texture->pixels[y * texture->stride + x];
}

static void texture_blit_func(texture_t* source_texture, texture_t* destination_texture, int sx, int sy, int dx, int dy) {
    color_t pixel = graphics_texture_pixel_get(source_texture, sx, sy);
    if (pixel == graphics_transparent_color_get()) return;

    graphics_texture_pixel_set(destination_texture, dx, dy, pixel);
}

void graphics_texture_blit(texture_t* source_texture, texture_t* destination_texture, rect_t* source_rect, rect_t* destination_rect) {
    graphics_blit(source_texture, destination_texture, source_rect, destination_rect, texture_blit_func);
}
