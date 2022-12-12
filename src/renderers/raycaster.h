#ifndef RENDERERS_RAYCASTER_H
#define RENDERERS_RAYCASTER_H

#include <mathc/mathc.h>

#include "../graphics.h"

void raycaster_render(mfloat_t* position, mfloat_t* direction, float fov, texture_t* map);

#endif
