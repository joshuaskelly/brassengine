#include <stdlib.h>
#include <string.h>

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
    free(texture);
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
    return texture->pixels[y * texture->width + x];
}

void graphics_texture_blit(texture_t* source_texture, texture_t* destination_texture, rect_t* source_rect, rect_t* destination_rect) {
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

    // TODO: Add checks for destination boundaries.
    for (dy = top, sy = source_rect->y; dy < bottom; dy++, sy += y_step) {
        for (dx = left, sx = source_rect->x; dx < right; dx++, sx += x_step) {
            color_t pixel = graphics_texture_get_pixel(source_texture, sx, sy);
            graphics_texture_set_pixel(destination_texture, dx, dy, pixel);
        }
    }
}

void graphics_init(void) {
    log_info("graphics init");

    render_texture = graphics_texture_new(
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

    clip_rect.x = 0;
    clip_rect.y = 0;
    clip_rect.width = RENDER_BUFFER_WIDTH;
    clip_rect.height = RENDER_BUFFER_HEIGHT;
}

void graphics_destroy(void) {
    free(palette);
    graphics_texture_free(render_texture);
}

texture_t* graphics_get_render_texture(void) {
    return render_texture;
}

uint32_t* graphics_palette_get(void) {
    return palette;
}

void graphics_palette_set(uint32_t* new_palette) {
    size_t size = 256 * sizeof(uint32_t);
    memmove(palette, new_palette, size);
}

void graphics_palette_clear(void) {
    for (int i = 0; i < 256; i++) {
        palette[i] = 0;
    }
}

void graphics_set_pixel(int x, int y, color_t color) {
    if (x < clip_rect.x || x >= clip_rect.x + clip_rect.width) return;
    if (y < clip_rect.y || y >= clip_rect.y + clip_rect.height) return;

    graphics_texture_set_pixel(render_texture, x, y, color);
}

void graphics_set_clipping_rectangle(int x, int y, int width, int height) {
    clip_rect.x = x;
    clip_rect.y = y;
    clip_rect.width = width;
    clip_rect.height = height;
}
