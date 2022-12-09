#ifndef RENDERERS_RAYCASTER_H
#define RENDERERS_RAYCASTER_H

#include <mathc/mathc.h>

#include "../graphics.h"

void raycaster_render(struct vec2* position, struct vec2* direction, texture_t* map);

#endif