#ifndef GRAPHICS_TYPES_H
#define GRAPHICS_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int x;
    int y;
    int width;
    int height;
} rect_t;

typedef uint8_t color_t;

typedef struct {
    int width;
    int height;
    int stride;
    bool is_subtexture;
    color_t* pixels;
} texture_t;

#endif