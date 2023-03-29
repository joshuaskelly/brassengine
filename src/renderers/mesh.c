#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include <math.h>

#include <mathc/mathc.h>

#include "../assets.h"
#include "../collections/float_array.h"
#include "../graphics.h"
#include "../math.h"
#include "../time.h"

#include "mesh.h"

typedef struct {
    mfloat_t v0[VEC2_SIZE];
    mfloat_t v1[VEC2_SIZE];
    mfloat_t v2[VEC2_SIZE];
    mfloat_t uv0[VEC2_SIZE];
    mfloat_t uv1[VEC2_SIZE];
    mfloat_t uv2[VEC2_SIZE];
} triangle_t;

mesh_t* mesh_mesh_new(void) {
    mesh_t* mesh;

    mesh = malloc(sizeof(mesh_t));
    mesh->vertices = float_array_new(0);
    mesh->uvs = float_array_new(0);
    mesh->indices = float_array_new(0);
    mesh->index_count = 0;

    return mesh;
}

void mesh_mesh_free(mesh_t* mesh) {
    float_array_free(mesh->vertices);
    float_array_free(mesh->uvs);
    float_array_free(mesh->indices);

    mesh->index_count = 0;
    mesh = NULL;
}

static int mesh_triangle_count(mesh_t* mesh) {
    return mesh->index_count / 3;
}

static triangle_t* mesh_get_triangle(mesh_t* mesh, triangle_t* triangle, int index) {
    float* vertices = mesh->vertices->data;
    float* uvs = mesh->uvs->data;
    float* indices = mesh->indices->data;

    int i = index * 6;
    int i0 = (int)(indices[i + 0]) * VEC2_SIZE;
    int i1 = (int)(indices[i + 1]) * VEC2_SIZE;
    int i2 = (int)(indices[i + 2]) * VEC2_SIZE;
    int i3 = (int)(indices[i + 3]) * VEC2_SIZE;
    int i4 = (int)(indices[i + 4]) * VEC2_SIZE;
    int i5 = (int)(indices[i + 5]) * VEC2_SIZE;

    mfloat_t* v0 = &vertices[i0];
    mfloat_t* v1 = &vertices[i1];
    mfloat_t* v2 = &vertices[i2];
    mfloat_t* uv0 = &uvs[i3];
    mfloat_t* uv1 = &uvs[i4];
    mfloat_t* uv2 = &uvs[i5];

    triangle->v0[0] = v0[0];
    triangle->v0[1] = v0[1];
    triangle->v1[0] = v1[0];
    triangle->v1[1] = v1[1];
    triangle->v2[0] = v2[0];
    triangle->v2[1] = v2[1];
    triangle->uv0[0] = uv0[0];
    triangle->uv0[1] = uv0[1];
    triangle->uv1[0] = uv1[0];
    triangle->uv1[1] = uv1[1];
    triangle->uv2[0] = uv2[0];
    triangle->uv2[1] = uv2[1];

    return triangle;
}

static void rasterize(triangle_t* triangle, texture_t* texture);
static mfloat_t vec2_cross(mfloat_t* v0, mfloat_t* v1);

void mesh_render(mesh_t* mesh, texture_t* texture) {
    for (int i = 0; i < mesh_triangle_count(mesh); i++) {
        triangle_t t;
        mesh_get_triangle(mesh, &t, i);
        rasterize(&t, texture);
    }
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
static void rasterize(triangle_t* triangle, texture_t* texture) {
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
    float bias0 = is_top_left(v1, v2) ? 0.0f : -0.001f;
    float bias1 = is_top_left(v2, v0) ? 0.0f : -0.001f;
    float bias2 = is_top_left(v0, v1) ? 0.0f : -0.001f;

    float delta_w0_col = v1[1] - v2[1];
    float delta_w1_col = v2[1] - v0[1];
    float delta_w2_col = v0[1] - v1[1];
    float delta_w0_row = v2[0] - v1[0];
    float delta_w1_row = v0[0] - v2[0];
    float delta_w2_row = v1[0] - v0[0];

    float area = edge_function(v1, v2, v0);
    float inverse_area = 1.0f / area;

    mfloat_t p[2] = { x_min + 0.5f, y_min + 0.5f };
    float w0_row = edge_function(v1, v2, p) + bias0;
    float w1_row = edge_function(v2, v0, p) + bias1;
    float w2_row = edge_function(v0, v1, p) + bias2;

    for (int y = y_min; y <= y_max; y++) {
        float w0 = w0_row;
        float w1 = w1_row;
        float w2 = w2_row;

        for (int x = x_min; x <= x_max; x++) {
            // Check if inside the triangle
            if (w0 >= 0 && w1 >= 0 && w2 >= 0) {
                // Calculate barycentric coords
                float alpha = w0 * inverse_area;
                float beta = w1 * inverse_area;
                float gamma = w2 * inverse_area;

                // Calculate st coords
                int s = (uv0[0] * alpha + uv1[0] * beta + uv2[0] * gamma) * texture->width;
                int t = (uv0[1] * alpha + uv1[1] * beta + uv2[1] * gamma) * texture->height;

                color_t c = graphics_texture_get_pixel(texture, s, t);

                graphics_set_pixel(x, y, c);
            }

            w0 += delta_w0_col;
            w1 += delta_w1_col;
            w2 += delta_w2_col;
        }

        w0_row += delta_w0_row;
        w1_row += delta_w1_row;
        w2_row += delta_w2_row;
    }
}

static mfloat_t vec2_cross(mfloat_t* v0, mfloat_t* v1) {
    return v0[0] * v1[1] - v0[1] * v1[0];
}
