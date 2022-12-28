#ifndef RENDERERS_RAYCASTER_H
#define RENDERERS_RAYCASTER_H

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#include <mathc/mathc.h>

#include "../graphics.h"

typedef struct {
    texture_t* walls;
} raycaster_map_t;

/**
 * Renders given map
 *
 * @param position Camera position vector
 * @param direction Camera direction vector
 * @param fov Camera field of view in degrees
 * @param map Map to render
 * @param render_texture Texture to render to. NULL will use default render target
 * @param render_rect Portion of render texture to render to. NULL will use entire render target
 */
void raycaster_render(
    mfloat_t* position,
    mfloat_t* direction,
    float fov,
    raycaster_map_t* map,
    texture_t* render_texture,
    rect_t* render_rect
);

/**
 * Returns a texture in the texture palette for given index.
 *
 * @param i Index of texture.
 * @return texture_t* Texture from palette.
 */
texture_t* raycaster_get_texture(int i);

/**
 * Sets a texture palette at given index to given texture.
 *
 * @param i Index in palette to set
 * @param texture Texture to set
 */
void raycaster_set_texture(int i, texture_t* texture);

/**
 * Gets shade table.
 *
 * @return texture_t* Shade table as a texture.
 */
texture_t* raycaster_shade_table_get(void);

/**
 * Sets shade table.
 *
 * @param texture Texture to use as the shade table.
 */
void raycaster_shade_table_set(texture_t* texture);

float raycaster_fog_distance_get(void);

void raycaster_fog_distance_set(float distance);

#endif
