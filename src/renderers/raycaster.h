#ifndef RENDERERS_RAYCASTER_H
#define RENDERERS_RAYCASTER_H

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#include <stdbool.h>
#include <mathc/mathc.h>

#include "../graphics.h"
#include "../collections/list.h"

typedef struct {
    texture_t* walls;
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

typedef struct {
    texture_t* render_texture;
    float* depth_buffer;

    struct {
        texture_t* shade_table;
        float fog_distance;
        bool draw_walls;
        bool draw_floors;
        bool draw_ceilings;
    } features;

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
