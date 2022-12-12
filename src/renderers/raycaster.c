#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include <mathc/mathc.h>

#include "../graphics.h"
#include "../log.h"
#include "draw.h"

#include "raycaster.h"

typedef texture_t map_t;
typedef color_t map_data_t;

static bool map_contains(map_t* map, int x, int y) {
    if (x < 0 || x >= map->width) return false;
    if (y < 0 || y >= map->height) return false;

    return true;
}

static map_data_t map_get_data(map_t* map, int x, int y) {
    //if (!map_contains(map, x, y)) return -1;

    return graphics_texture_get_pixel(map, x, y);
}

static bool map_is_solid(map_t* map, int x, int y) {
    return map_get_data(map, x, y) > 0;
}

typedef struct {
    vec2_t position;
    float distance;
    map_data_t* data;
    bool was_vertical;
} ray_hit_info_t;

typedef struct {
    vec2_t position;
    vec2_t direction;
    ray_hit_info_t hit_info;
} ray_t;

static void ray_set(ray_t* ray, vec2_t position, vec2_t direction) {
    vec2_assign(ray->position, position);
    vec2_assign(ray->direction, direction);

    vec2_assign(ray->hit_info.position, position);
    ray->hit_info.distance = FLT_MAX;
    ray->hit_info.data = NULL;
    ray->hit_info.was_vertical = false;
}

static void ray_rotate(ray_t* ray, float f) {
    vec2_rotate(ray->direction, ray->direction, f);
}

static void ray_draw(ray_t* ray) {
    draw_line(
        ray->position[0] * 32,
        ray->position[1] * 32,
        ray->hit_info.position[0] * 32,
        ray->hit_info.position[1] * 32,
        12
    );
}

static void draw_grid_cell(int x, int y, color_t c) {
    draw_rectangle(x * 32, y * 32, 32, 32, c);
}

static void ray_cast(ray_t* ray, map_t* map) {
    // Horizontal checks
    if (ray->direction[1] != 0.0f)
    {
        float y_intercept;
        float x_intercept;
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

        // Use ratio to determine x-value of first horizontal intersection from
        // ray origin
        x_intercept = (ray->direction[0] * ratio) + ray->position[0];

        // This also happens to be the distance to the first intersection
        distance = ratio;

        // Track horizontal grid intersections
        vec2_t intersection;
        vec2_assign(intersection, ray->direction);
        vec2_multiply_f(intersection, intersection, ratio);
        vec2_add(intersection, intersection, ray->position);

        // After the first intersection, all following intersections are at a
        // regular interval
        vec2_t step;
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
            int j = floorf(intersection[1]) + ray_direction_offset;

            // DEBUG: Show path of checks
            draw_grid_cell(i, j, 79);

            graphics_set_pixel(
                intersection[0] * 32,
                intersection[1] * 32,
                14
            );

            // Check if we've hit a wall
            if (map_is_solid(map, i, j)) {
                // DEBUG: Show chosen cell for hit
                draw_grid_cell(i, j, 14);
                ray->hit_info.position[0] = intersection[0];
                ray->hit_info.position[1] = intersection[1];
                ray->hit_info.distance = distance;

                break;
            }

            // Otherwise next iteration
            vec2_add(intersection, intersection, step);
            distance += fabsf(inv_y);
        }
    }
}

void raycaster_render(vec2_t position, vec2_t direction, float fov, texture_t* map) {
    texture_t* render_texture = graphics_get_render_texture();

    // Draw background
    for (int y = 0; y < map->height; y++) {
        for (int x = 0; x < map->width; x++) {
            color_t c = graphics_texture_get_pixel(map, x, y);
            draw_filled_rectangle(x *  32, y * 32, 32, 32, c);
        }
    }

    // Draw grid
    for (int i = 0; i < render_texture->height; i += 32) {
        draw_line(0, i, render_texture->width, i, 8);
    }

    for (int i = 0; i < render_texture->width; i += 32) {
        draw_line(i, 0, i, render_texture->height, 8);
    }

    float fov_rads = fov * M_PI / 180.0f;

    vec2_normalize(direction, direction);

    int ray_count = render_texture->width;
    //int ray_count = 4;
    float fov_inc = fov_rads / (ray_count - 1);

    ray_t ray;
    ray_set(&ray, position, direction);
    ray_rotate(&ray, fov_rads * -0.5f);

    if (false) {
    // Cast all rays
    for (int i = 0; i < ray_count; i++) {
        ray_cast(&ray, map);
        ray_draw(&ray);

        ray_rotate(&ray, fov_inc);
    }
    }
    else {
        // DEBUG: Test a single ray.
        ray_set(&ray, position, direction);
        ray_cast(&ray, map);
        ray_draw(&ray);

        if (ray.hit_info.distance != FLT_MAX) {
            log_info("distance: %f", ray.hit_info.distance);
        }
    }

    if (true) {
        vec2_t left_bound;
        vec2_assign(left_bound, direction);
        vec2_rotate(left_bound, left_bound, fov_rads * -0.5f);
        vec2_multiply_f(left_bound, left_bound, 32.0f);

        draw_line(
            position[0] * 32,
            position[1] * 32,
            position[0] * 32 + left_bound[0] * 32,
            position[1] * 32 + left_bound[1] * 32,
            31
        );

        vec2_t right_bound;
        vec2_assign(right_bound, direction);
        vec2_rotate(right_bound, right_bound, fov_rads * 0.5f);
        vec2_multiply_f(right_bound, right_bound, 32.0f);

        draw_line(
            position[0] * 32,
            position[1] * 32,
            position[0] * 32 + right_bound[0] * 32,
            position[1] * 32 + right_bound[1] * 32,
            31
        );
    }

    // Draw camera position
    graphics_texture_set_pixel(render_texture, position[0] * 32, position[1] * 32, 12);
}
