#include <math.h>
#include <stdlib.h>
#include <string.h>

#include <mathc/mathc.h>

#include "../graphics.h"
#include "../log.h"
#include "../math.h"

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
    float depth;
} triangle_t;

static int compare(const void* a, const void* b) {
    triangle_t* t0 = (triangle_t*)a;
    triangle_t* t1 = (triangle_t*)b;

    float d0 = t0->depth;
    float d1 = t1->depth;

    if (d0 > d1) {
        return -1;
    }
    else if(d1 > d0) {
        return 1;
    }

    return 0;
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
        vec4_assign(result[*count].normal, triangle->normal);

        ++(*count);
    }
}

typedef uint32_t indices_t[3];
typedef mfloat_t mfloat3_t[VEC3_SIZE];

void mesh_renderer_render(mesh_renderer_t* renderer, mesh_mesh_t* mesh, mfloat_t* model_view, mfloat_t* projection) {
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

    mfloat_t matrix[MAT4_SIZE];
    mat4_multiply(matrix, projection, model_view);

    mfloat_t normal_transform[MAT4_SIZE];
    mat4_assign(normal_transform, model_view);
    mat4_inverse(normal_transform, normal_transform);
    mat4_transpose(normal_transform, normal_transform);

    // View space transformation
    for (triangle_t* triangle = first; triangle < last; triangle++) {
        // Transform position
        vec4_multiply_mat4(triangle->v0.position, triangle->v0.position, model_view);
        vec4_multiply_mat4(triangle->v1.position, triangle->v1.position, model_view);
        vec4_multiply_mat4(triangle->v2.position, triangle->v2.position, model_view);

        // Transform normal
        mfloat_t* normal = &triangle->normal;
        vec4_multiply_mat4(normal, normal, normal_transform);
        normal[3] = 0;
        vec4_normalize(normal, normal);
    }

    triangle_t t2[10000];
    uint32_t t2_count = 0;

    mfloat_t dir[VEC3_SIZE] = {};

    // Back-face culling
    for (triangle_t* triangle = first; triangle < last; triangle++) {
        vec3_normalize(dir, &triangle->v0.position);
        if (vec3_dot(triangle->normal, dir) > 0.0f) continue;

        t2[t2_count] = *triangle;
        t2_count++;
    }

    first = t2;
    last = &t2[t2_count];

    // Projection transformation
    for (triangle_t* triangle = first; triangle < last; triangle++) {
        vec4_multiply_mat4(triangle->v0.position, triangle->v0.position, projection);
        vec4_multiply_mat4(triangle->v1.position, triangle->v1.position, projection);
        vec4_multiply_mat4(triangle->v2.position, triangle->v2.position, projection);
    }

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

    // Perspective divide
    for (triangle_t* triangle = first; triangle < last; triangle++) {
        vec3_divide_f(triangle->v0.position, triangle->v0.position, triangle->v0.position[3]);
        vec3_divide_f(triangle->v1.position, triangle->v1.position, triangle->v1.position[3]);
        vec3_divide_f(triangle->v2.position, triangle->v2.position, triangle->v2.position[3]);

        triangle->depth = (triangle->v0.position[3] + triangle->v1.position[3] + triangle->v2.position[3]) / 3.0f;
    }

    // Sort by depth (Painter's Algorithm)
    qsort(t3, t3_count, sizeof(triangle_t), compare);

    mfloat_t half_window[VEC4_SIZE];
    vec4(
        half_window,
        (renderer->render_texture->width - 1) / 2.0f,
        (renderer->render_texture->height - 1) / 2.0f,
        0.0f,
        1.0f
    );

    mfloat_t height = renderer->render_texture->height - 1;

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

    mfloat_t light[VEC4_SIZE] = {0, 0, 1, 0};

    // Draw triangles
    for (triangle_t* triangle = first; triangle < last; triangle++) {
        float f = vec3_dot(triangle->normal, light);
        f = clamp(f, 0, 1);
        int color = remap(0.0f, 1.0f, 0, 15, f);

        graphics_draw_filled_triangle(
            renderer->render_texture,
            triangle->v0.position[0], triangle->v0.position[1],
            triangle->v1.position[0], triangle->v1.position[1],
            triangle->v2.position[0], triangle->v2.position[1],
            color
        );
    }
}

void mesh_renderer_camera(mesh_renderer_t* renderer, mfloat_t* view, mfloat_t* projection) {
    mat4_assign(renderer->view, view);
    mat4_assign(renderer->projection, projection);
}
