#ifndef RENDERERS_VOXELSPACE_H
#define RENDERERS_VOXELSPACE_H

#include <mathc/mathc.h>

typedef struct {
    mfloat_t position[VEC2_SIZE];
    mfloat_t direction[VEC2_SIZE];
    float fov;
    float far;
} voxelspace_camera_t;

#endif