#ifndef ASSETS_H
#define ASSETS_H

#include <stdbool.h>
#include <stdint.h>

#include "graphics.h"

/**
 * @brief Initialize assets system.
 */
void assets_init(void);

/**
 * @brief Destroy assets system.
 */
void assets_destroy(void);

bool assets_load(void);

void assets_unload(void);

void assets_reload(void);

texture_t* assets_get_texture(int index);

typedef struct {
    int frame_count;
    texture_t** frames;
    uint32_t* palette;
} gif_t;

gif_t* assets_gif_load(const char* filename);

void assets_gif_free(gif_t* gif);

#endif