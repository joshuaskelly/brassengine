#ifndef ASSETS_H
#define ASSETS_H

#include <stdbool.h>
#include <stdint.h>

#include "graphics.h"

typedef struct {
    int version;
    char* script;
    uint32_t palette[256];
    texture_t** textures;
    int texture_count;
} archive_t;

/**
 * @brief Initialize assets system.
 */
void assets_init(void);

/**
 * @brief Destroy assets system.
 */
void assets_destroy(void);

bool assets_load_archive(char* filename);

void assets_unload_archive(void);

char* assets_get_script(void);

uint32_t* assets_get_palette(void);

texture_t* assets_get_texture(int index);

#endif