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
    mfloat_t position[VEC2_SIZE];
    float distance;
    map_data_t* data;
    bool was_vertical;
} ray_hit_info_t;

typedef struct {
    mfloat_t position[VEC2_SIZE];
    mfloat_t direction[VEC2_SIZE];
    ray_hit_info_t hit_info;
} ray_t;

static void ray_set(ray_t* ray, mfloat_t* position, mfloat_t* direction) {
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
        float y_intercept;
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

        // Use ratio to determine y-value of first vertical intersection from
        // ray origin
        y_intercept = (ray->direction[1] * ratio) + ray->position[1];

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

                break;
            }

            // Otherwise next iteration
            vec2_add(intersection, intersection, step);
            distance += fabsf(inv_x);
        }
    }
}

void raycaster_render(mfloat_t* position, mfloat_t* direction, float fov, texture_t* map) {
    texture_t* render_texture = graphics_get_render_texture();

    const int ray_count = render_texture->width;
    const float fov_rads = fov * M_PI / 180.0f;
    const float distance_to_projection_plane = (render_texture->width / 2.0f) / tanf(fov_rads / 2.0f);

    // Ensure direction is normalized
    vec2_normalize(direction, direction);

    /*
     * Casting rays along a plane in front of the camera.
     *
     * To determine the ray directions:
     *
     * 1. Two points on opposite sides of the view frustum are found. These are
     *    called left bound and right bound.
     *
     * 2. A vector between the two bounds is found to determine the direction
     *    along the plane the rays will be cast.
     *
     * 3. The vector along the plane is scaled by 1 / render texture width to
     *    determine the step of each ray.
     *
     *            \_                 _/
     * (left bound) l<----plane---->r (right bound)
     *               \_           _/
     *                 \_       _/
     *                   \_   _/
     *                     \ /
     *                      c (camera position)
     */

    // Get left bound
    mfloat_t left_bound[VEC2_SIZE];
    vec2_assign(left_bound, direction);
    vec2_rotate(left_bound, left_bound, fov_rads * -0.5f);

    mfloat_t step[VEC2_SIZE];

    // Get right bound
    vec2_assign(step, direction);
    vec2_rotate(step, step, fov_rads * 0.5f);

    // Get vector along plane
    vec2_subtract(step, step, left_bound);

    // Scale by inverse texture width to get step vector
    vec2_multiply_f(step, step, 1.0f / render_texture->width);

    ray_t ray;
    ray_set(&ray, position, direction);
    ray_rotate(&ray, fov_rads * -0.5f);

    // Cast all rays
    for (int i = 0; i < ray_count; i++) {
        ray_cast(&ray, map);

        // Calculate wall height
        mfloat_t hit_vector[VEC2_SIZE];
        vec2_multiply_f(hit_vector, ray.direction, ray.hit_info.distance);
        float corrected_distance = vec2_dot(hit_vector, direction);

        float wall_height = 1.0f / corrected_distance * distance_to_projection_plane;
        float half_wall_height = wall_height / 2.0f;

        color_t c = 15;
        if (ray.hit_info.was_vertical) {
            c = 21;
        }

        // Draw wall strip
        draw_line(
            i,
            render_texture->height / 2.0f - half_wall_height,
            i,
            render_texture->height / 2.0f + half_wall_height,
            c
        );

        // Orient ray to next position along plane
        vec2_assign(ray.direction, step);
        vec2_multiply_f(ray.direction, ray.direction, i + 1);
        vec2_add(ray.direction, ray.direction, left_bound);
        vec2_normalize(ray.direction, ray.direction);

        // Clear out hit info
        ray.hit_info.distance = FLT_MAX;
    }
}
