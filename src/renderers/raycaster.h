#ifndef RENDERERS_RAYCASTER_H
#define RENDERERS_RAYCASTER_H

#include <mathc/mathc.h>

#include "../graphics.h"

void raycaster_render(mfloat_t* position, mfloat_t* direction, float fov, texture_t* map);

texture_t* raycaster_get_texture(int i);

void raycaster_set_texture(int i, texture_t* texture);

texture_t* raycaster_shade_table_get(void);

void raycaster_shade_table_set(texture_t* texture);

#endif
