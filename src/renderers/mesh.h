#ifndef RENDERERS_MESH_H
#define RENDERERS_MESH_H

#include <mathc/mathc.h>

#include "../graphics.h"

typedef struct {
    mfloat_t* vertices;
    mfloat_t* uvs;
    int* indices;
    size_t index_count;
} mesh_t;

mesh_t* mesh_mesh_new(void);

void mesh_mesh_free(mesh_t* mesh);

void mesh_render(mesh_t* mesh, texture_t* texture);

#endif
