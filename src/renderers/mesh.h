#ifndef RENDERERS_MESH_H
#define RENDERERS_MESH_H

typedef struct {
    mfloat_t* vertices;
    mfloat_t* uvs;
    int* indices;
    size_t index_count;
} mesh_t;

void mesh_render(void);

#endif
