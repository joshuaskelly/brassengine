#ifndef RENDERERS_RAYCASTER_H
#define RENDERERS_RAYCASTER_H

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#include <mathc/mathc.h>

#include "../graphics.h"
#include "../collections/list.h"

typedef struct {
    mfloat_t position[VEC2_SIZE];
    mfloat_t direction[VEC2_SIZE];
    float fov;
} raycaster_camera_t;

typedef struct {
    texture_t* texture;
    mfloat_t position[VEC2_SIZE];
    float distance;
} raycaster_sprite_t;

typedef struct {
    texture_t* walls;
    list_t* sprites;
} raycaster_map_t;

/**
 * Creates a new map.
 *
 * @return raycaster_map_t* Newly created map.
 */
raycaster_map_t* raycaster_map_new(void);

/**
 * Frees a map.
 *
 * @param map Map to free.
 */
void raycaster_map_free(raycaster_map_t* map);

/**
 * Adds a sprite to a map.
 *
 * @param map Map to add sprite to.
 * @param sprite Sprite to add.
 */
void raycaster_map_add_sprite(raycaster_map_t* map, raycaster_sprite_t* sprite);

/**
 * Removes a sprite from a map.
 *
 * @param map Map to remove sprite from.
 * @param sprite Sprite to remove.
 */
void raycaster_map_remove_sprite(raycaster_map_t* map, raycaster_sprite_t* sprite);

/**
 * Renders given map
 *
 * @param camera Camera to render from
 * @param map Map to render
 * @param render_texture Texture to render to. NULL will use default render target
 * @param render_rect Portion of render texture to render to. NULL will use entire render target
 */
void raycaster_render(
    raycaster_camera_t* camera,
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

typedef struct {
    texture_t* render_texture;
    float* depth_buffer;
    texture_t* shade_table;
    float fog_distance;
    struct {
        mfloat_t position[VEC2_SIZE];
        mfloat_t direction[VEC2_SIZE];
        float fov;
    } camera;
} raycaster_renderer_t;

raycaster_renderer_t* raycaster_renderer_new(texture_t* render_texture);

void raycaster_renderer_free(raycaster_renderer_t* renderer);

void raycaster_renderer_clear_color(raycaster_renderer_t* renderer, color_t color);

void raycaster_renderer_clear_depth(raycaster_renderer_t* renderer, float depth);

void raycaster_renderer_camera(raycaster_renderer_t* renderer, mfloat_t* position, mfloat_t* direction, float fov);

void raycaster_renderer_render_map(raycaster_renderer_t* renderer, raycaster_map_t* map, texture_t** palette);

void raycaster_renderer_render_sprite(raycaster_renderer_t* renderer, texture_t* sprite, mfloat_t* position);

#endif
