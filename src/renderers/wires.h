#ifndef RENDERERS_WIRES_H
#define RENDERERS_WIRES_H

#include <mathc/mathc.h>

#include "../graphics.h"

typedef struct {
    size_t line_count;
    int* indices;
    color_t* colors;
    size_t vertex_count;
    mfloat_t* vertices;
} wires_line_buffer_t;

wires_line_buffer_t* wires_line_buffer_new(int line_count, int vertex_count);

struct plane {
    mfloat_t point[VEC3_SIZE];
    mfloat_t normal[VEC3_SIZE];
};

typedef struct {
    mfloat_t view_matrix[MAT4_SIZE];
    mfloat_t projection_matrix[MAT4_SIZE];
    struct {
        struct plane near;
        struct plane far;
        struct plane left;
        struct plane right;
        struct plane top;
        struct plane bottom;
    } clip_planes;
    struct {
        int lines_rendered;
    } statistics;
} wires_renderer_t;

wires_renderer_t* wires_renderer_new(void);

void wires_renderer_free(wires_renderer_t* renderer);

void wires_renderer_start(wires_renderer_t* renderer);

void wires_renderer_stop(wires_renderer_t* renderer);

void wires_renderer_render(wires_renderer_t* renderer, mfloat_t* model_matrix, wires_line_buffer_t* lines);

void wires_renderer_camera(wires_renderer_t* renderer, float near, float far, float fov, mfloat_t view_matrix[MAT4_SIZE]);

#endif
