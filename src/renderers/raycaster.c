#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <mathc/mathc.h>

#include "../assets.h"
#include "../graphics.h"
#include "../log.h"
#include "../math.h"
#include "draw.h"

#include "raycaster.h"

typedef int map_data_t;

/**
 * Shade table. Used for shading a given pixel. The y-coordinate corresponds
 * to original color and the x-coordinate corresponds to desired brightness.
 */
static texture_t* shade_table;

static float fog_distance = 32.0f;

static raycaster_renderer_t* active_renderer;

raycaster_map_t* raycaster_map_new(int width, int height) {
    raycaster_map_t* map = (raycaster_map_t*)malloc(sizeof(raycaster_map_t));
    map->width = width;
    map->height = height;

    size_t size = width * height;

    map->walls = (int*)malloc(size * sizeof(int));
    map->floors = (int*)malloc(size * sizeof(int));
    map->ceilings = (int*)malloc(size * sizeof(int));

    return map;
}

void raycaster_map_free(raycaster_map_t* map) {
    free(map->walls);
    map->walls = NULL;
    free(map->floors);
    map->floors = NULL;
    free(map->ceilings);
    map->ceilings = NULL;

    free(map);
    map = NULL;
}

/**
 * Determine if given point is contained in the map bounds.
 *
 * @param map Map to check against
 * @param x Point x-coordinate
 * @param y Point y-coordinate
 * @return true If point is contained in map bounds.
 * @return false If point is outside of map bounds.
 */
static bool map_contains(raycaster_map_t* map, int x, int y) {
    if (x < 0 || x >= map->width) return false;
    if (y < 0 || y >= map->height) return false;

    return true;
}

/**
 * Get map wall data at a given point
 *
 * @param map Map to check
 * @param x Point x-coordinate
 * @param y Point y-coordinate
 * @return map_data_t
 */
static map_data_t map_get_wall(raycaster_map_t* map, int x, int y) {
    if (x < 0 || x >= map->width) return 0;
    if (y < 0 || y >= map->height) return 0;

    return map->walls[y * map->width + x];
}

/**
 * Get map floor data at a given point
 *
 * @param map Map to check
 * @param x Point x-coordinate
 * @param y Point y-coordinate
 * @return map_data_t
 */
static map_data_t map_get_floor(raycaster_map_t* map, int x, int y) {
    if (x < 0 || x >= map->width) return 0;
    if (y < 0 || y >= map->height) return 0;

    return map->floors[y * map->width + x];
}


/**
 * Get map ceiling data at a given point
 *
 * @param map Map to check
 * @param x Point x-coordinate
 * @param y Point y-coordinate
 * @return map_data_t
 */
static map_data_t map_get_ceiling(raycaster_map_t* map, int x, int y) {
    if (x < 0 || x >= map->width) return 0;
    if (y < 0 || y >= map->height) return 0;

    return map->ceilings[y * map->width + x];
}

/**
 * Check if map is solid at a given point.
 *
 * @param map Map to check
 * @param x Point x-coordinate
 * @param y Point y-coordinate
 * @return true If map is solid at given point.
 * @return false If map is not solid at given point.
 */
static bool map_is_solid(raycaster_map_t* map, int x, int y) {
    return map_get_wall(map, x, y) > 0;
}

typedef struct {
    mfloat_t position[VEC2_SIZE];
    float distance;
    map_data_t data;
    bool was_vertical;
} ray_hit_info_t;

typedef struct {
    mfloat_t position[VEC2_SIZE];
    mfloat_t direction[VEC2_SIZE];
    ray_hit_info_t hit_info;
} ray_t;

/**
 * Reset ray hit info to initial state.
 *
 * @param hit_info Hit info to reset.
 */
static void ray_hit_info_reset(ray_hit_info_t* hit_info) {
    hit_info->distance = FLT_MAX;
    hit_info->data = 0;
    hit_info->was_vertical = false;
}

static void ray_set(ray_t* ray, mfloat_t* position, mfloat_t* direction) {
    vec2_assign(ray->position, position);
    vec2_assign(ray->direction, direction);

    vec2_assign(ray->hit_info.position, position);
    ray_hit_info_reset(&ray->hit_info);
}

/**
 * Casts a ray. The ray hit info will be updated with the result of the cast.
 *
 * @param ray Ray to cast.
 * @param map Map to cast against.
 */
static void ray_cast(ray_t* ray, raycaster_map_t* map) {
    // Horizontal checks
    if (ray->direction[1] != 0.0f)
    {
        float y_intercept;
        bool ray_facing_down = ray->direction[1] > 0;
        float distance;
        float inv_y = 1.0f / ray->direction[1];

        // Determine y-value of first horizontal intersection from ray origin
        y_intercept = floorf(ray->position[1]);
        if (ray_facing_down) {
            y_intercept += 1;
        }

        // Calculate ratio of y-components
        float ratio = (y_intercept - ray->position[1]) * inv_y;

        // This also happens to be the distance to the first intersection
        distance = ratio;

        // Track horizontal grid intersections
        mfloat_t intersection[VEC2_SIZE];
        vec2_assign(intersection, ray->direction);
        vec2_multiply_f(intersection, intersection, ratio);
        vec2_add(intersection, intersection, ray->position);

        // After the first intersection, all following intersections are at a
        // regular interval
        mfloat_t step[VEC2_SIZE];
        vec2_assign(step, ray->direction);
        vec2_multiply_f(step, step, inv_y);
        if (!ray_facing_down) {
            vec2_multiply_f(step, step, -1.0f);
        }

        int ray_direction_offset = ray_facing_down ? 0 : -1;

        int bound = map->height;

        for(int s = 0; s < bound; s++) {
            // Ensure we are still inside map bounds
            if (!map_contains(map, intersection[0], intersection[1])) break;

            int i = floorf(intersection[0]);
            int j = floorf(intersection[1] + 0.001f) + ray_direction_offset;

            // Check if we've hit a wall
            if (map_is_solid(map, i, j)) {
                ray->hit_info.position[0] = intersection[0];
                ray->hit_info.position[1] = intersection[1];
                ray->hit_info.distance = distance;
                ray->hit_info.was_vertical = false;
                ray->hit_info.data = map_get_wall(map, i, j);

                break;
            }

            // Otherwise next iteration
            vec2_add(intersection, intersection, step);
            distance += fabsf(inv_y);
        }
    }

    // Vertical checks
    if (ray->direction[0] != 0.0f)
    {
        float x_intercept;
        bool ray_facing_right = ray->direction[0] > 0;
        float distance;
        float inv_x = 1.0f / ray->direction[0];

        // Determine x-value of first vertical intersection from ray origin
        x_intercept = floorf(ray->position[0]);
        if (ray_facing_right) {
            x_intercept += 1;
        }

        // Calculate ratio of x-components
        float ratio = (x_intercept - ray->position[0]) * inv_x;

        // This also happens to be the distance to the first intersection
        distance = ratio;

        // Track horizontal grid intersections
        mfloat_t intersection[VEC2_SIZE];
        vec2_assign(intersection, ray->direction);
        vec2_multiply_f(intersection, intersection, ratio);
        vec2_add(intersection, intersection, ray->position);

        // After the first intersection, all following intersections are at a
        // regular interval
        mfloat_t step[VEC2_SIZE];
        vec2_assign(step, ray->direction);
        vec2_multiply_f(step, step, inv_x);
        if (!ray_facing_right) {
            vec2_multiply_f(step, step, -1.0f);
        }

        int ray_direction_offset = ray_facing_right ? 0 : -1;

        int bound = map->width;

        for(int s = 0; s < bound; s++) {
            // Ensure we are still inside map bounds
            if (!map_contains(map, intersection[0], intersection[1])) break;

            // Early out if further than horizontal intersection
            if (distance > ray->hit_info.distance) break;

            int i = floorf(intersection[0] + 0.001f) + ray_direction_offset;
            int j = floorf(intersection[1]);

            // Check if we've hit a wall
            if (map_is_solid(map, i, j)) {
                ray->hit_info.position[0] = intersection[0];
                ray->hit_info.position[1] = intersection[1];
                ray->hit_info.distance = distance;
                ray->hit_info.was_vertical = true;
                ray->hit_info.data = map_get_wall(map, i, j);

                break;
            }

            // Otherwise next iteration
            vec2_add(intersection, intersection, step);
            distance += fabsf(inv_x);
        }
    }
}

/**
 * Shades given pixel to given brightness using the shade table.
 *
 * @param color Original color
 * @param brightness Amount to shade pixel. 1.0 = full bright 0.0 = full dark
 * @return color_t Shaded color
 */
static color_t shade_pixel(color_t color, float brightness) {
    if (!shade_table) return color;
    if (color >= shade_table->height) return color;

    brightness = clamp(brightness, 0.0f, 1.0f);
    brightness = 1.0f - brightness;

    const int amount = brightness * (shade_table->width - 1);

    return graphics_texture_get_pixel(
        shade_table,
        amount,
        color
    );
}

/**
 * Get brightness for given distance.
 *
 * @param distance Distance from camera
 * @return float Brightness where 1.0 is full bright and 0.0 is full dark.
 */
static float get_distance_based_brightness(float distance) {
    return 1.0f - distance / fog_distance;
}

static void set_depth_buffer_pixel(raycaster_renderer_t* renderer, int x, int y, float depth) {
    texture_t* texture = renderer->render_texture;
    float* depth_buffer = renderer->depth_buffer;

    if (x < 0 || x >= texture->width) return;
    if (y < 0 || y >= texture->height) return;

    depth_buffer[y * texture->width + x] = depth;
}

static float get_depth_buffer_pixel(raycaster_renderer_t* renderer, int x, int y) {
    texture_t* texture = renderer->render_texture;
    float* depth_buffer = renderer->depth_buffer;

    if (x < 0 || x >= texture->width) return FLT_MAX;
    if (y < 0 || y >= texture->height) return FLT_MAX;

    return depth_buffer[y * texture->width + x];
}

/**
 * Draw a single pixel wide vertical wall strip.
 *
 * @param wall_texture Wall texture
 * @param destination_texture Texture to draw wall to
 * @param x Wall x-coordinate on destination texture
 * @param y0 Top of wall y-coordinate on destination texture
 * @param y1 Bottom of wall y-coordinate on destination texture
 * @param offset Wall texture x-coordinate offset.
 * @param brightness How light/dark to shade wall.
 */
static void draw_wall_strip(texture_t* wall_texture, texture_t* destination_texture, int x, int y0, int y1, float offset, float brightness, float depth) {
    const int length = y1 - y0;
    const int start = y0 < 0 ? abs(y0) : 0;
    const int bottom = destination_texture->height;

    const int s = wall_texture->width * offset;
    const float t_step = wall_texture->height / (float)length;
    float t = start * t_step;

    for (int i = start; i < length; i++) {
        int y = y0 + i;
        if (y >= bottom) break;

        color_t c = graphics_texture_get_pixel(wall_texture, s, t);
        t += t_step;
        if (c == graphics_transparent_color_get()) continue;

        float d = get_depth_buffer_pixel(active_renderer, x, y);
        if (d <= depth) continue;

        set_depth_buffer_pixel(active_renderer, x, y, depth);

        c = shade_pixel(c, brightness);
        graphics_texture_set_pixel(destination_texture, x, y, c);

    }
}

/** Depth of currently rendering sprite. */
static float sprite_depth = FLT_MAX;

/**
 * Pixel drawing function that respects ray depth.
 *
 * @param source_texture Texture to copy from
 * @param destination_texture Texture to copy to
 * @param sx Source x-coordinate
 * @param sy Source y-coordinate
 * @param dx Destination x-coordinate
 * @param dy Destination y-coordinate
 */
static void sprite_depth_blit_func(texture_t* source_texture, texture_t* destination_texture, int sx, int sy, int dx, int dy) {
    rect_t* clip_rect = graphics_get_clipping_rectangle();
    if (dx < clip_rect->x || dx >= clip_rect->x + clip_rect->width) return;
    if (dy < clip_rect->y || dy >= clip_rect->y + clip_rect->height) return;

    float depth = sprite_depth;
    float d = get_depth_buffer_pixel(active_renderer, dx, dy);
    if (d <= depth) return;

    color_t pixel = graphics_texture_get_pixel(source_texture, sx, sy);
    if (pixel == graphics_transparent_color_get()) return;

    set_depth_buffer_pixel(active_renderer, dx, dy, depth);

    float brightness = get_distance_based_brightness(sprite_depth);
    pixel = shade_pixel(pixel, brightness);

    graphics_texture_set_pixel(destination_texture, dx, dy, pixel);
}

raycaster_renderer_t* raycaster_renderer_new(texture_t* render_texture) {
    raycaster_renderer_t* renderer = (raycaster_renderer_t*)malloc(sizeof(raycaster_renderer_t));

    if (!render_texture) {
        render_texture = graphics_get_render_texture();
    }

    size_t size = render_texture->width * render_texture->height;

    renderer->render_texture = render_texture;
    renderer->depth_buffer = (float*)malloc(size * sizeof(float));
    renderer->features.shade_table = NULL;
    renderer->features.fog_distance = 32.0f;
    renderer->features.draw_walls = true;
    renderer->features.draw_floors = true;
    renderer->features.draw_ceilings = true;
    renderer->features.horizontal_wall_brightness = 1.0f;
    renderer->features.vertical_wall_brightness = 0.5f;

    vec2(renderer->camera.position, 0, 0);
    vec2(renderer->camera.direction, 0, 0);
    renderer->camera.fov = 90.0f;

    return renderer;
}

void raycaster_renderer_free(raycaster_renderer_t* renderer) {
    free(renderer->depth_buffer);
    renderer->depth_buffer = NULL;

    free(renderer);
    renderer = NULL;
}

void raycaster_renderer_clear_color(raycaster_renderer_t* renderer, color_t color) {
    graphics_texture_clear(renderer->render_texture, color);
}

void raycaster_renderer_clear_depth(raycaster_renderer_t* renderer, float depth) {
    size_t size = renderer->render_texture->width * renderer->render_texture->height;

    for (int i = 0; i < size; i++) {
        renderer->depth_buffer[i] = depth;
    }
}

void raycaster_renderer_camera(raycaster_renderer_t* renderer, mfloat_t* position, mfloat_t* direction, float fov) {
    vec2_assign(renderer->camera.position, position);
    vec2_assign(renderer->camera.direction, direction);
    renderer->camera.fov = fov;
}

void raycaster_renderer_render_map(raycaster_renderer_t* renderer, raycaster_map_t* map, texture_t** palette) {
    if (!renderer->render_texture) {
        return;
    }

    active_renderer = renderer;

    mfloat_t* position = renderer->camera.position;
    mfloat_t* direction = renderer->camera.direction;
    float fov = renderer->camera.fov;

    texture_t* render_texture = renderer->render_texture;
    if (!render_texture) {
        render_texture = graphics_get_render_texture();
    }

    shade_table = renderer->features.shade_table;
    fog_distance = renderer->features.fog_distance;

    const float width = render_texture->width;
    const float height = render_texture->height;

    // Ensure direction is normalized
    vec2_normalize(direction, direction);

    /*
     * Casting rays along a plane in front of the camera.
     *
     * To determine the ray directions:
     *
     * 1. Determine distance to the projection plane. This distance will ensure
     *    that width of the projection plane bounded by our fov is the same
     *    width as the render texture.
     *
     * 2. Find left bound. This is the point on the projection plane where the
     *    left fov bound intersects the plane.
     *
     * 3. Find step vector. Because the projection plane width is the same width
     *    as the render texture, each step vector is of length one. The step
     *    vector is just the negated tangent to the camera direction.
     *
     *           \_                    _/
     * (left bound) _l<----plane---->_/
     *                \_           _/
     *                  \_   ^   _/
     *                    \_ |<---(camera direction)
     *                      \|/
     *                       c (camera position)
     */

    // 1. Determine distance to the projection plane.
    const float distance_to_projection_plane = (width / 2.0f) / tanf(to_radians(fov) / 2.0f);

    // Calculate step vector, we need it to move along the projection plane
    mfloat_t step[VEC2_SIZE];
    vec2_tangent(step, direction);
    vec2_negative(step, step);

    // 2. Calculate left bound.

    // left_bound = position + (direction * distance_to_projection_plane) - (step * width / 2)
    mfloat_t left_bound[VEC2_SIZE];
    vec2_multiply_f(left_bound, direction, distance_to_projection_plane);
    vec2_multiply_f(step, step, width * 0.5f);
    vec2_subtract(left_bound, left_bound, step);

    // 3. Find step vector
    vec2_tangent(step, direction);
    vec2_negative(step, step);

    // Track where the next ray needs to point
    mfloat_t next[VEC2_SIZE];
    vec2_add(next, left_bound, position);

    // Initialize ray to point to left bound
    ray_t ray;
    ray_set(&ray, position, ray.direction);
    vec2_subtract(ray.direction, next, position);
    vec2_normalize(ray.direction, ray.direction);

    mfloat_t floor_step[VEC2_SIZE];
    mfloat_t floor_next[VEC2_SIZE];

    float horizontal_wall_brightness = renderer->features.horizontal_wall_brightness;
    float vertical_wall_brightness = renderer->features.vertical_wall_brightness;

    // Draw walls
    if (renderer->features.draw_walls && map->walls) {
        for (int i = 0; i < width; i++) {
            ray_cast(&ray, map);

            // Calculate wall height
            mfloat_t hit_vector[VEC2_SIZE];
            vec2_multiply_f(hit_vector, ray.direction, ray.hit_info.distance);
            float corrected_distance = vec2_dot(hit_vector, direction);

            float wall_height = 1.0f / corrected_distance * distance_to_projection_plane;
            float half_wall_height = wall_height / 2.0f;

            // Calculate the texture normalized horizontal offset (u-coordinate).
            float offset = 0.0f;
            if (ray.hit_info.was_vertical) {
                offset = frac(ray.hit_info.position[1]);

                // Flip texture to maintain correct orienation
                if (ray.direction[0] < 0) {
                    offset = 1.0f - offset;
                }
            }
            else {
                offset = frac(ray.hit_info.position[0]);

                // Flip texture to maintain correct orienation
                if (ray.direction[1] > 0) {
                    offset = 1.0f - offset;
                }
            }

            texture_t* wall_texture = palette[ray.hit_info.data];
            if (wall_texture) {
                float brightness = get_distance_based_brightness(ray.hit_info.distance);
                // Darken vertically aligned walls.
                brightness *= ray.hit_info.was_vertical ? vertical_wall_brightness : horizontal_wall_brightness;

                draw_wall_strip(
                    wall_texture,
                    render_texture,
                    i,
                    height / 2.0f - half_wall_height - 0.5f,
                    height / 2.0f + half_wall_height + 1.5f,
                    offset,
                    brightness,
                    corrected_distance
                );
            }

            // Orient ray to next position along plane
            vec2_add(next, next, step);
            vec2_subtract(ray.direction, next, position);
            vec2_normalize(ray.direction, ray.direction);

            // Clear out hit info
            ray_hit_info_reset(&ray.hit_info);
        }
    }

    // Draw floor/ceiling
    for (int j = height / 2.0f; j < height; j++) {
        // Calculate distance from render texture y-coordinate
        float wall_height = 2.0f * j - height;
        float distance = distance_to_projection_plane / wall_height;

        // Distance at horizon line is infinity
        if (isinf(distance)) continue;

        float scale = 1.0f / wall_height;

        // Determine floor left bound
        vec2_multiply_f(floor_next, left_bound, scale);
        vec2_add(floor_next, floor_next, position);

        // Determine floor horizontal step.
        vec2_multiply_f(floor_step, step, scale);

        float brightness = get_distance_based_brightness(distance);

        // Draw current scanline for both floor and ceiling
        for (int i = 0; i < width; i++) {
            int tx = (int)floor_next[0];
            int ty = (int)floor_next[1];

            // Draw floor
            float d = get_depth_buffer_pixel(active_renderer, i, j);
            if (d > distance && renderer->features.draw_floors && map->floors) {
                int index = map_get_floor(map, tx, ty);
                texture_t* texture = palette[index];

                if (texture) {
                    int x = frac(floor_next[0]) * texture->width;
                    int y = frac(floor_next[1]) * texture->height;

                    color_t color = graphics_texture_get_pixel(texture, x, y);

                    // Floor
                    graphics_texture_set_pixel(
                        render_texture, i, j, shade_pixel(color, brightness)
                    );
                    set_depth_buffer_pixel(active_renderer, i, j, d);
                }
            }

            // Draw ceiling
            d = get_depth_buffer_pixel(active_renderer, i, height - j - 1);
            if (d > distance && renderer->features.draw_ceilings && map->ceilings) {
                int index = map_get_ceiling(map, tx, ty);
                texture_t* texture = palette[index];

                if (texture) {
                    int x = frac(floor_next[0]) * texture->width;
                    int y = frac(floor_next[1]) * texture->height;

                    color_t color = graphics_texture_get_pixel(texture, x, y);

                    // Ceiling
                    graphics_texture_set_pixel(
                        render_texture, i, height - j - 1, shade_pixel(color, brightness)
                    );
                    set_depth_buffer_pixel(active_renderer, i, height - j - 1, d);
                }
            }

            vec2_add(floor_next, floor_next, floor_step);
        }
    }
}

void raycaster_renderer_render_sprite(raycaster_renderer_t* renderer, texture_t* sprite, mfloat_t* position) {
    if (!renderer->render_texture) return;
    if (!sprite) return;

    active_renderer = renderer;

    texture_t* render_texture = renderer->render_texture;
    mfloat_t* direction = renderer->camera.direction;
    mfloat_t* camera_position = renderer->camera.position;

    shade_table = renderer->features.shade_table;
    fog_distance = renderer->features.fog_distance;

    const float width = render_texture->width;
    const float height = render_texture->height;

    const float fov = renderer->camera.fov;
    const float distance_to_projection_plane = (width / 2.0f) / tanf(to_radians(fov) / 2.0f);

    // Calculate step vector, we need it to move along the projection plane
    mfloat_t step[VEC2_SIZE];
    vec2_tangent(step, direction);
    vec2_negative(step, step);

    // Calculate sprite projected distance
    mfloat_t dir[VEC2_SIZE];
    vec2_subtract(dir, position, camera_position);
    float distance = vec2_dot(direction, dir);

    // Cull sprites outside near/far planes
    if (distance < 0) return;
    if (distance >= fog_distance) return;

    // Frustum culling
    mfloat_t d[VEC2_SIZE];
    vec2_assign(d, dir);
    vec2_normalize(d, d);

    float ang = acos(vec2_dot(direction, d));
    if (ang > (to_radians(fov) / 2.0f) + 0.175f) return;

    // Scale to put point on projection plane.
    vec2_multiply_f(dir, dir, distance_to_projection_plane / distance);

    // Find x offset
    float x_offset = vec2_dot(dir, step);

    float s_height = 1.0f / distance * distance_to_projection_plane;
    float half_height = s_height / 2.0f;

    rect_t rect = {
        (width / 2.0f) + x_offset - half_height,
        (height / 2.0f) - half_height,
        s_height,
        s_height
    };

    // Set sprite depth for blit func
    sprite_depth = distance;

    // Draw sprite
    graphics_blit(
        sprite,
        render_texture,
        NULL,
        &rect,
        sprite_depth_blit_func
    );
}

/**
 * Test intersection for given line segment and ray having origin at 0, 0
 *
 * @param result intersection point if intersection occurs.
 * @param l first endpoint
 * @param r second endpoint
 * @param ray direction of ray
 * @return true if intersection occurs, false otherwise.
 */
static bool intersect_camera_ray(mfloat_t* result, mfloat_t* a, mfloat_t* b, mfloat_t* ray) {
    mfloat_t r[VEC2_SIZE];
    vec2_normalize(r, ray);

    float x1 = a[0];
    float y1 = a[1];
    float x2 = b[0];
    float y2 = b[1];
    float x3 = r[0];
    float y3 = r[1];
    float x4 = 0;
    float y4 = 0;

    float x12 = x1 - x2;
    float x34 = x3 - x4;
    float y12 = y1 - y2;
    float y34 = y3 - y4;

    float c = x12 * y34 - y12 * x34;

    if (fabs(c) < 0.01) {
        return false;
    }

    float t = x1 * y2 - y1 * x2;
    float u = x3 * y4 - y3 * x4;

    float x = (t * x34 - u * x12) / c;
    float y = (t * y34 - u * y12) / c;

    result[0] = x;
    result[1] = y;

    return true;
}

void raycaster_renderer_render_sprite_oriented(raycaster_renderer_t* renderer, texture_t* sprite, mfloat_t* position, mfloat_t* forward) {
    if (!renderer->render_texture) return;
    if (!sprite) return;

    active_renderer = renderer;

    texture_t* render_texture = renderer->render_texture;
    mfloat_t* direction = renderer->camera.direction;
    mfloat_t* camera_position = renderer->camera.position;

    const float width = render_texture->width;
    const float height = render_texture->height;
    const float half_width = width / 2.0f;
    const float half_height = height / 2.0f;

    const float fov = renderer->camera.fov;
    const float distance_to_projection_plane = half_width / tanf(to_radians(fov) / 2.0f);

    mfloat_t angle = vec2_angle(direction);
    angle -= MPI_2;

    // Calculate camera matrix
    mfloat_t m[MAT3_SIZE];
    mat3_identity(m);
    mat3_rotation_z(m, angle);
    m[6] = camera_position[0];
    m[7] = camera_position[1];
    mat3_inverse(m, m);

    // Transform sprite position
    mfloat_t p[VEC3_SIZE];
    vec3(p, position[0], position[1], 1.0f);
    vec3_multiply_mat3(p, p, m);
    p[2] = 0;

    // Calculate sprite tangent
    mfloat_t tangent[VEC3_SIZE];
    vec3(tangent, 0, 0, 1.0f);
    vec2_tangent(tangent, forward);
    vec2_normalize(tangent, tangent);
    m[6] = 0;
    m[7] = 0;
    vec3_multiply_mat3(tangent, tangent, m);
    tangent[2] = 0;

    // Calculate half-tangent
    mfloat_t half_tangent[VEC3_SIZE];
    vec3_divide_f(half_tangent, tangent, 2.0f);

    /** First sprite endpoint. */
    mfloat_t a[VEC3_SIZE];
    vec3_subtract(a, p, half_tangent);

    /** Second sprite endpoint. */
    mfloat_t b[VEC3_SIZE];
    vec3_add(b, p, half_tangent);

    // Near culling
    if (a[1] <= 0 && b[1] <= 0) return;

    /** Clipped left sprite endpoint. */
    mfloat_t l[VEC3_SIZE];
    vec3_assign(l, a);

    /** Clipped right sprite endpoint. */
    mfloat_t r[VEC3_SIZE];
    vec3_assign(r, b);

    // Right clip plane
    {
        mfloat_t clip_plane_normal[VEC2_SIZE];
        vec2(clip_plane_normal, -half_width, distance_to_projection_plane);
        vec2_normalize(clip_plane_normal, clip_plane_normal);
        vec2_tangent(clip_plane_normal, clip_plane_normal);

        /** Clipped left endpoint projected onto clip plane normal.*/
        float aa = vec2_dot(clip_plane_normal, l);

        /** Clipped right endpoint projected onto clip plane normal.*/
        float bb = vec2_dot(clip_plane_normal, r);

        // Both endpoints are outside clipping plane.
        if (aa <= 0 && bb <= 0) return;

        // Second endpoint is outside clipping plane. Back-facing sprite.
        if (aa > 0 && bb < 0) {
            float f = aa / (aa - bb);
            vec3_subtract(tangent, l, r);
            vec3_multiply_f(tangent, tangent, f);
            vec3_subtract(r, l, tangent);
        }
        // First endpoint is outside clipping plane. Front-facing sprite.
        else if (aa < 0 && bb > 0) {
            float f = bb / (bb - aa);
            vec3_subtract(tangent, l, r);
            vec3_multiply_f(tangent, tangent, f);
            vec3_add(l, r, tangent);
        }
    }

    // Left clip plane
    {
        mfloat_t clip_plane_normal[VEC2_SIZE];
        vec2(clip_plane_normal, half_width, distance_to_projection_plane);
        vec2_normalize(clip_plane_normal, clip_plane_normal);
        vec2_tangent(clip_plane_normal, clip_plane_normal);
        vec2_multiply_f(clip_plane_normal, clip_plane_normal, -1.0f);

        /** Clipped left endpoint projected onto clip plane normal.*/
        float aa = vec2_dot(clip_plane_normal, l);

        /** Clipped right endpoint projected onto clip plane normal.*/
        float bb = vec2_dot(clip_plane_normal, r);

        // Both endpoints are outside clipping plane.
        if (aa <= 0 && bb <= 0) return;

        // Second endpoint is outside clipping plane. Back-facing sprite.
        if (bb < 0 && aa > 0) {
            float f = fabs(bb) / (aa - bb);
            vec3_subtract(tangent, l, r);
            vec3_multiply_f(tangent, tangent, f);
            vec3_add(r, r, tangent);
        }
        // First endpoint is outside clipping plane. Front-facing sprite.
        else if (aa < 0 && bb > 0) {
            float f = fabs(aa) / (bb - aa);
            vec3_subtract(tangent, l, r);
            vec3_multiply_f(tangent, tangent, f);
            vec3_subtract(l, l, tangent);
        }
    }

    // Flip
    int left_bound = l[0] * distance_to_projection_plane / l[1] - 0.5f;
    int right_bound = r[0] * distance_to_projection_plane / r[1] + 0.5f;

    if (right_bound > left_bound) {
        float swap = b[0];
        b[0] = a[0];
        a[0] = swap;
        swap = b[1];
        b[1] = a[1];
        a[1] = swap;

        swap = r[0];
        r[0] = l[0];
        l[0] = swap;
        swap = r[1];
        r[1] = l[1];
        l[1] = swap;

        swap = right_bound;
        right_bound = left_bound;
        left_bound = (int)swap;
    }

    // Recalculate tangent. This is to ensure correct texture mapping.
    vec3_subtract(tangent, b, a);

    // Clamp to visible bounds
    left_bound = (int)fmin(left_bound, half_width);
    right_bound = (int)fmax(right_bound, -half_width);

    mfloat_t intersection[VEC2_SIZE];
    mfloat_t ray[VEC2_SIZE];
    vec2(ray, 0, distance_to_projection_plane);

    // Draw sprite
    for (int i = left_bound; i > right_bound; i--) {
        ray[0] = i;
        if (intersect_camera_ray(intersection, l, r, ray)) {
            float distance = intersection[1];

            if (distance <= 0) continue;

            float brightness = get_distance_based_brightness(distance);
            float half_sprite_height = ((distance_to_projection_plane / distance) * 0.5f);
            vec3_subtract(p, intersection, a);
            p[2] = 0;
            float offset = vec3_dot(p, tangent);

            draw_wall_strip(
                sprite,
                render_texture,
                half_width - i,
                half_height - half_sprite_height - 0.5f,
                half_height + half_sprite_height + 1.5f,
                offset,
                brightness,
                distance
            );
        }
    }
}
