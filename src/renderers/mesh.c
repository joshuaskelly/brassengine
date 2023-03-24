#include <stdbool.h>
#include <stdint.h>

#include <math.h>

#include <mathc/mathc.h>

#include "../assets.h"
#include "../graphics.h"
#include "../math.h"
#include "../time.h"

#include "mesh.h"

typedef struct {
    mfloat_t *v0;
    mfloat_t *v1;
    mfloat_t *v2;
    mfloat_t *uv0;
    mfloat_t *uv1;
    mfloat_t *uv2;
} triangle_t;

static void rasterize(triangle_t* triangle);
static mfloat_t* vec2_rotate_around(mfloat_t* result, mfloat_t* v0, mfloat_t* v1, float f);
static mfloat_t vec2_cross(mfloat_t* v0, mfloat_t* v1);

void mesh_render(void) {
    mfloat_t v0[VEC2_SIZE] = {20, 20};
    mfloat_t v1[VEC2_SIZE] = {100, 20};
    mfloat_t v2[VEC2_SIZE] = {20, 100};
    mfloat_t v3[VEC2_SIZE] = {100, 100};

    mfloat_t uv0[VEC2_SIZE] = {0, 0};
    mfloat_t uv1[VEC2_SIZE] = {1, 0};
    mfloat_t uv2[VEC2_SIZE] = {0, 1};
    mfloat_t uv3[VEC2_SIZE] = {1, 1};

    triangle_t t0 = {
        .v0 = v0,
        .v1 = v1,
        .v2 = v2,
        .uv0 = uv0,
        .uv1 = uv1,
        .uv2 = uv2
    };

    triangle_t t1 = {
        .v0 = v3,
        .v1 = v2,
        .v2 = v1,
        .uv0 = uv3,
        .uv1 = uv2,
        .uv2 = uv1
    };

    mfloat_t c[VEC2_SIZE] = {64, 64};

    float angle = time_since_init() / 1000.0f * 0.2f;

    vec2_rotate_around(v0, v0, c, angle);
    vec2_rotate_around(v1, v1, c, angle);
    vec2_rotate_around(v2, v2, c, angle);
    vec2_rotate_around(v3, v3, c, angle);

    rasterize(&t0);
    rasterize(&t1);
}

static int edge_function(mfloat_t* a, mfloat_t* b, mfloat_t* p) {
    mfloat_t ab[VEC2_SIZE];
    vec2_subtract(ab, b, a);

    mfloat_t ap[VEC2_SIZE];
    vec2_subtract(ap, p, a);

    return vec2_cross(ab, ap);
}

static bool is_top_left(mfloat_t* a, mfloat_t* b) {
    mfloat_t edge[VEC2_SIZE];
    vec2_subtract(edge, b, a);

    bool is_top = edge[1] == 0 && edge[0] > 0;
    bool is_left = edge[1] < 0;

    return is_top || is_left;
}

/**
 * Rasterize triangle.
 *
 * @param triangle
 */
static void rasterize(triangle_t* triangle) {
    mfloat_t* v0 = triangle->v0;
    mfloat_t* v1 = triangle->v1;
    mfloat_t* v2 = triangle->v2;
    mfloat_t* uv0 = triangle->uv0;
    mfloat_t* uv1 = triangle->uv1;
    mfloat_t* uv2 = triangle->uv2;

    // Find triangle
    int x_min = min(min(v0[0], v1[0]), v2[0]);
    int y_min = min(min(v0[1], v1[1]), v2[1]);
    int x_max = max(max(v0[0], v1[0]), v2[0]);
    int y_max = max(max(v0[1], v1[1]), v2[1]);

    // Biases for fill rule
    float bias0 = is_top_left(v1, v2) ? 0.0f : 0.0001f;
    float bias1 = is_top_left(v2, v0) ? 0.0f : 0.0001f;
    float bias2 = is_top_left(v0, v1) ? 0.0f : 0.0001f;

    float inverse_area = 1.0f / edge_function(v1, v2, v0);

    texture_t* tex = assets_get_texture("texture.gif");

    for (int y = y_min; y <= y_max; y++) {
        for (int x = x_min; x <= x_max; x++) {
            mfloat_t p[2] = { x + 0.5f, y + 0.5f };

            float w0 = edge_function(v1, v2, p);
            float w1 = edge_function(v2, v0, p);
            float w2 = edge_function(v0, v1, p);

            // Check if p is contained within the triangle
            if (w0 < bias0 || w1 < bias1 || w2 < bias2) continue;

            // Calculate barycentric coords
            float alpha = w0 * inverse_area;
            float beta = w1 * inverse_area;
            float gamma = 1 - (alpha + beta);

            // Calculate st coords
            int s = (uv0[0] * alpha + uv1[0] * beta + uv2[0] * gamma) * tex->width;
            int t = (uv0[1] * alpha + uv1[1] * beta + uv2[1] * gamma) * tex->height;

            color_t c = graphics_texture_get_pixel(tex, s, t);

            graphics_set_pixel(x, y, c);
        }
    }
}

static mfloat_t* vec2_rotate_around(mfloat_t* result, mfloat_t* v0, mfloat_t* v1, float f) {
    mfloat_t cs = MCOS(f);
	mfloat_t sn = MSIN(f);
	mfloat_t x = v0[0] - v1[0];
	mfloat_t y = v0[1] - v1[1];
	result[0] = (x * cs - y * sn) + v1[0];
	result[1] = (x * sn + y * cs) + v1[1];

	return result;
}

static mfloat_t vec2_cross(mfloat_t* v0, mfloat_t* v1) {
    return v0[0] * v1[1] - v0[1] * v1[0];
}
