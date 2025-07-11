#ifndef RENDERERS_RAYCASTER_H
#define RENDERERS_RAYCASTER_H

#include <stdbool.h>
#include <mathc/mathc.h>

#include "../graphics.h"
#include "../threads.h"
#include "../collections/list.h"

typedef struct {
    int width;
    int height;

    int* walls;
    int* floors;
    int* ceilings;
} raycaster_map_t;

/**
 * Creates a new map.
 *
 * @return raycaster_map_t* Newly created map.
 */
raycaster_map_t* raycaster_map_new(int width, int height);

/**
 * Frees a map.
 *
 * @param map Map to free.
 */
void raycaster_map_free(raycaster_map_t* map);

typedef struct {
    texture_t* render_texture;
    float* depth_buffer;
    thread_pool_t* pool;

    struct {
        texture_t* shade_table;
        float fog_distance;
        bool draw_walls;
        bool draw_floors;
        bool draw_ceilings;
        float horizontal_wall_brightness;
        float vertical_wall_brightness;
        float pixels_per_unit;
        bool threaded;
    } features;

    struct {
        mfloat_t position[VEC2_SIZE];
        mfloat_t direction[VEC2_SIZE];
        float fov;
    } camera;
} raycaster_renderer_t;

/**
 * Creates a new renderer.
 *
 * @return raycast_renderer_t Newly created renderer.
 */
raycaster_renderer_t* raycaster_renderer_new(texture_t* render_texture);

/**
 * Frees a renderer.
 *
 * @param renderer Renderer to free.
 */
void raycaster_renderer_free(raycaster_renderer_t* renderer);

/**
 * Clears color buffer for given color.
 *
 * @param renderer Renderer to clear color buffer.
 * @param color Clear color.
 */
void raycaster_renderer_clear_color(raycaster_renderer_t* renderer, color_t color);

/**
 * Clears depth buffer for given color.
 *
 * @param renderer Renderer to clear depth buffer.
 * @param depth Clear depth.
 */
void raycaster_renderer_clear_depth(raycaster_renderer_t* renderer, float depth);

/**
 * Set camera data.
 *
 * @param renderer Renderer to set camera data for.
 * @param position Camera position.
 * @param direction Camera direction.
 * @param fov Camera fov.
 */
void raycaster_renderer_camera(raycaster_renderer_t* renderer, mfloat_t* position, mfloat_t* direction, float fov);

/**
 * Render given map.
 *
 * @param renderer Renderer to render to.
 * @param map Map to render.
 * @param palette Texture array look up table.
 */
void raycaster_renderer_render_map(raycaster_renderer_t* renderer, raycaster_map_t* map, texture_t** palette);

/**
 * Render given texture as a billboarded sprite.
 *
 * @param renderer Renderer to render to.
 * @param sprite Texture to render.
 * @param position Sprite position.
 */
void raycaster_renderer_render_sprite(raycaster_renderer_t* renderer, texture_t* sprite, mfloat_t* position);

/**
 * Render given texture as an oriented sprite.
 *
 * @param renderer Renderer to render to.
 * @param sprite Texture to render.
 * @param position Sprite position.
 * @param forward Sprite forward vector.
 */
void raycaster_renderer_render_sprite_oriented(raycaster_renderer_t* renderer, texture_t* sprite, mfloat_t* position, mfloat_t* forward);

#endif
