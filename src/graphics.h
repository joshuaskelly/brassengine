#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdint.h>

#define RENDER_BUFFER_WIDTH 320
#define RENDER_BUFFER_HEIGHT 200

typedef uint8_t color_t;

typedef struct {
    color_t* pixels;
    int width;
    int height;
} texture_t;

texture_t* texture_new(int width, int height, color_t* pixels);
texture_t* texture_load(char* path);
void texture_free(texture_t* texture);
void texture_clear(texture_t* texture, color_t color);
void texture_set_pixel(texture_t* texture, int x, int y, color_t color);
color_t texture_get_pixel(texture_t* texture, int x, int y);

void graphics_init(void);
void graphics_destroy(void);
texture_t* graphics_get_render_texture(void);
uint32_t* graphics_get_palette(void);

#endif
