#ifndef RENDERERS_WIRES_H
#define RENDERERS_WIRES_H

#include <mathc/mathc.h>

#include "../graphics.h"
#include "../collections/float_array.h"
#include "../collections/int_array.h"

typedef struct {
    int_array_t* indices;
    int_array_t* colors;
    float_array_t* vertices;
} wires_line_buffer_t;

/**
 * Creates a new line buffer.
 *
 * @param line_count Number of lines
 * @param vertex_count Number of vertices
 * @return wires_line_buffer_t* Newly created line buffer.
 */
wires_line_buffer_t* wires_line_buffer_new(int line_count, int vertex_count);

/**
 * Frees a line buffer.
 *
 * @param lines Line buffer to free.
 */
void wires_line_buffer_free(wires_line_buffer_t* lines);

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

/**
 * Creates a new wire renderer.
 *
 * @return wires_renderer_t*
 */
wires_renderer_t* wires_renderer_new(void);

/**
 * Frees a wire renderer.
 *
 * @param renderer
 */
void wires_renderer_free(wires_renderer_t* renderer);

/**
 * Begin render.
 *
 * @param renderer
 */
void wires_renderer_start(wires_renderer_t* renderer);

/**
 * End render.
 *
 * @param renderer
 */
void wires_renderer_stop(wires_renderer_t* renderer);

/**
 * Render given line buffer.
 *
 * @param renderer
 * @param model_matrix Transform to apply to line buffer.
 * @param lines Line buffer to render.
 */
void wires_renderer_render(wires_renderer_t* renderer, mfloat_t* model_matrix, wires_line_buffer_t* lines);

/**
 * Set camera data.
 *
 * @param renderer
 * @param near Near clip plane.
 * @param far Far clip plane.
 * @param fov Camera fov.
 * @param view_matrix View matrix to transform world geometry.
 */
void wires_renderer_camera(wires_renderer_t* renderer, float near, float far, float fov, mfloat_t view_matrix[MAT4_SIZE]);

#endif
