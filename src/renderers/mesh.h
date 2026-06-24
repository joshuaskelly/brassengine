#ifndef RENDERERS_MESH_H
#define RENDERERS_MESH_H

#include <stdint.h>

#include <mathc/mathc.h>

#include "../graphics.h"

typedef struct {
    uint32_t triangle_count;
    uint32_t vertex_count;
    uint32_t* indices;
    mfloat_t* vertices;
    mfloat_t* normals;
    color_t* colors;
} mesh_mesh_t;

mesh_mesh_t* mesh_mesh_new(void);

void mesh_mesh_free(mesh_mesh_t* mesh);

typedef struct {
    mfloat_t view[MAT4_SIZE];
    mfloat_t projection[MAT4_SIZE];
    texture_t* render_texture;
} mesh_renderer_t;

mesh_renderer_t* mesh_renderer_new(texture_t* render_texture);

void mesh_renderer_free(mesh_renderer_t* renderer);

void mesh_renderer_render(mesh_renderer_t* renderer, mesh_mesh_t* mesh, mfloat_t* matrix);

void mesh_renderer_camera(mesh_renderer_t* renderer, mfloat_t* view, mfloat_t* projection);

#endif
