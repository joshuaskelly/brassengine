#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <mathc/mathc.h>

#include "../graphics.h"
#include "../log.h"

#include "mesh.h"

mesh_mesh_t* mesh_mesh_new(void) {
    mesh_mesh_t* mesh = (mesh_mesh_t*)malloc(sizeof(mesh_mesh_t));

    mesh->triangle_count = 0;
    mesh->vertex_count = 0;
    mesh->indices = NULL;
    mesh->vertices = NULL;
    mesh->normals = NULL;
    mesh->colors = NULL;

    return mesh;
}

void mesh_mesh_free(mesh_mesh_t* mesh) {
    if (mesh->indices) {
        free(mesh->indices);
        mesh->indices = NULL;
    }

    if (mesh->vertices) {
        free(mesh->vertices);
        mesh->vertices = NULL;
    }

    if (mesh->normals) {
        free(mesh->normals);
        mesh->normals = NULL;
    }

    if (mesh->colors) {
        free(mesh->colors);
        mesh->colors = NULL;
    }

    free(mesh);
    mesh = NULL;
}

mesh_renderer_t* mesh_renderer_new(texture_t* render_texture) {
    mesh_renderer_t* renderer = (mesh_renderer_t*)malloc(sizeof(mesh_renderer_t));

    if (!render_texture) {
        renderer->render_texture = graphics_render_texture_get();
    }
    else {
        renderer->render_texture = render_texture;
    }

    mat4_identity(renderer->view);
    mat4_identity(renderer->projection);

    return renderer;
}

void mesh_renderer_free(mesh_renderer_t* renderer) {
    renderer->render_texture = NULL;
    mat4_zero(renderer->view);
    mat4_zero(renderer->projection);

    free(renderer);
    renderer = NULL;
}

typedef struct {
    mfloat_t position[VEC4_SIZE];
} vertex_t;

static vertex_t* vertex_copy(vertex_t* result, vertex_t* v) {
    vec4_assign(result->position, v->position);
    return result;
}

static vertex_t* vertex_lerp(vertex_t* result, vertex_t* v0, vertex_t* v1, float f) {
    vec4_lerp(result->position, v0->position, v1->position, f);
    return result;
}

typedef struct {
    vertex_t v0;
    vertex_t v1;
    vertex_t v2;
    mfloat_t normal[VEC4_SIZE];
    color_t color;
} triangle_t;

static void triangle_transform(triangle_t* triangle, mfloat_t* matrix) {
    vec4_multiply_mat4(triangle->v0.position, triangle->v0.position, matrix);
    vec4_multiply_mat4(triangle->v1.position, triangle->v1.position, matrix);
    vec4_multiply_mat4(triangle->v2.position, triangle->v2.position, matrix);
    vec4_multiply_mat4(triangle->normal, triangle->normal, matrix);
}

static void triangle_calculate_normal(triangle_t* triangle) {
    mfloat_t a[VEC4_SIZE];
    mfloat_t b[VEC4_SIZE];

    vec4_subtract(a, triangle->v1.position, triangle->v0.position);
    vec4_subtract(b, triangle->v2.position, triangle->v0.position);
    vec3_cross(triangle->normal, a, b);
    triangle->normal[3] = 0.0f;
}

static void triangle_clip(triangle_t* result, int* count, triangle_t* triangle) {
    const int plane_count = 4;
    mfloat_t planes[VEC4_SIZE][plane_count] = {
        { 0, -1, 0, -1},
        { 1,  0, 0, -1},
        {-1,  0, 0, -1},
        { 0,  1, 0, -1},
    };

    vertex_t v0[16] = {
        triangle->v0,
        triangle->v1,
        triangle->v2
    };
    int length = 3;

    vertex_t v1[16];

    vertex_t* output;
    output = v1;
    int l = 0;

    vertex_t* input;
    input = v0;

    for (int i = 0; i < plane_count; i++) {
        mfloat_t* plane = &planes[i];

        for (int j = 0; j < length; j++) {
            vertex_t a = input[j];
            vertex_t b = input[(j + 1) % length];

            float da = vec4_dot(a.position, plane);
            float db = vec4_dot(b.position, plane);

            if (da > 0 && db < 0) {
                float t = da / (da - db);
                vertex_t* v = output + l;
                vertex_lerp(v, &a, &b, t);
                l++;
            }
            else if (db > 0 && da < 0) {
                float t = db / (db - da);
                vertex_t* v = output + l;
                vertex_lerp(v, &b, &a, t);
                l++;
            }

            if (db < 0) {
                vertex_t* v = output + l;
                vertex_copy(v, &b);
                l++;
            }
        }

        // swap
        length = l;
        l = 0;

        vertex_t* t = input;
        input = output;
        output = t;
    }

    *count = 0;

    if (length < 3) {
        return;
    }

    for (int i = 2; i < length; i++) {
        result[*count].v0 = input[0];
        result[*count].v1 = input[i - 1];
        result[*count].v2 = input[i];

        result[*count].color = triangle->color;
        vec4_assign(result[*count].normal,triangle->normal);

        ++(*count);
    }
}

typedef uint32_t indices_t[3];
typedef mfloat_t mfloat3_t[VEC3_SIZE];

void mesh_renderer_render(mesh_renderer_t* renderer, mesh_mesh_t* mesh, mfloat_t* matrix) {
    indices_t* indices = (indices_t*)mesh->indices;
    mfloat3_t* vertices = (mfloat3_t*)mesh->vertices;
    mfloat3_t* normals = (mfloat3_t*)mesh->normals;

    // Create triangles
    triangle_t triangles[mesh->triangle_count];
    for (int i = 0; i < mesh->triangle_count; i++) {
        uint32_t index_0 = indices[i][0];
        uint32_t index_1 = indices[i][1];
        uint32_t index_2 = indices[i][2];

        mfloat_t* v0 = vertices[index_0];
        mfloat_t* v1 = vertices[index_1];
        mfloat_t* v2 = vertices[index_2];

        mfloat_t* normal = normals[i];

        triangles[i] = (triangle_t){
            .v0 = {.position = {v0[0], v0[1], v0[2], 1.0f}},
            .v1 = {.position = {v2[0], v2[1], v2[2], 1.0f}},
            .v2 = {.position = {v1[0], v1[1], v1[2], 1.0f}},
            .normal = {normal[0], normal[1], normal[2], 0.0f},
            .color = mesh->colors[i]
        };
    }

    triangle_t* first = triangles;
    triangle_t* last = &triangles[mesh->triangle_count];

    // Transform triangles
    for (triangle_t* triangle = first; triangle < last; triangle++) {
        triangle_transform(triangle, matrix);
    }

    triangle_t t2[10000];
    uint32_t t2_count = 0;

    // Back face culling
    for (triangle_t* triangle = first; triangle < last; triangle++) {
        if (vec3_dot(triangle->normal, triangle->v0.position) > 0.0f) continue;

        t2[t2_count] = *triangle;
        t2_count++;
    }

    //log_info("visible tris: %i", t2_count);

    first = t2;
    last = &t2[t2_count];

    //*
    triangle_t t3[10000];
    uint32_t t3_count = 0;

    // Clipping
    triangle_t clipped[16];
    for (triangle_t* triangle = first; triangle < last; triangle++) {
        int count;
        triangle_clip(clipped, &count, triangle);

        for (int i = 0; i < count; i++) {
            t3[t3_count++] = clipped[i];
        }
    }

    first = t3;
    last = &t3[t3_count];
    //*/

    // Perspective divide
    for (triangle_t* triangle = first; triangle < last; triangle++) {
        float w = triangle->v0.position[3];
        vec4_divide_f(triangle->v0.position, triangle->v0.position, w);
        triangle->v0.position[3] = w;

        w = triangle->v1.position[3];
        vec4_divide_f(triangle->v1.position, triangle->v1.position, w);
        triangle->v1.position[3] = w;

        w = triangle->v2.position[3];
        vec4_divide_f(triangle->v2.position, triangle->v2.position, w);
        triangle->v2.position[3] = w;
    }

    mfloat_t half_window[VEC4_SIZE];
    vec4(
        half_window,
        (renderer->render_texture->width - 1) / 2.0f,
        (renderer->render_texture->height - 1) / 2.0f,
        0.0f,
        1.0f
    );

    mfloat_t height = renderer->render_texture->height;

    // Screen space
    for (triangle_t* triangle = first; triangle < last; triangle++) {
        vec4_add_f(triangle->v0.position, triangle->v0.position, 1.0f);
        vec4_multiply(triangle->v0.position, triangle->v0.position, half_window);
        triangle->v0.position[1] = height - triangle->v0.position[1];

        vec4_add_f(triangle->v1.position, triangle->v1.position, 1.0f);
        vec4_multiply(triangle->v1.position, triangle->v1.position, half_window);
        triangle->v1.position[1] = height - triangle->v1.position[1];

        vec4_add_f(triangle->v2.position, triangle->v2.position, 1.0f);
        vec4_multiply(triangle->v2.position, triangle->v2.position, half_window);
        triangle->v2.position[1] = height - triangle->v2.position[1];
    }


    // Draw triangles
    for (triangle_t* triangle = first; triangle < last; triangle++) {
        graphics_draw_triangle(
            renderer->render_texture,
            triangle->v0.position[0], triangle->v0.position[1],
            triangle->v1.position[0], triangle->v1.position[1],
            triangle->v2.position[0], triangle->v2.position[1],
            triangle->color
        );
    }
}

void mesh_renderer_camera(mesh_renderer_t* renderer, mfloat_t* view, mfloat_t* projection) {
    mat4_assign(renderer->view, view);
    mat4_assign(renderer->projection, projection);
}
