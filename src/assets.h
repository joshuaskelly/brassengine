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

const char* assets_get_script(void);

#endif