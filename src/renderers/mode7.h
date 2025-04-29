#ifndef RENDERERS_MODE7_H
#define RENDERERS_MODE7_H

#include <stdbool.h>

#include <mathc/mathc.h>

#include "../graphics.h"

typedef enum {
    WRAP_NONE = 0,
    WRAP_REPEAT,
    WRAP_CLAMP
} mode7_wrap_mode_t;

typedef struct {
    texture_t* render_texture;
    mfloat_t matrix[MAT3_SIZE];

    struct {
        mode7_wrap_mode_t wrap_mode;
    } features;
} mode7_renderer_t;

typedef void(*mode7_callback_t)(int);

/**
 * Creates a new renderer.
 *
 * @return mode7_renderer_t Newly created renderer.
 */
mode7_renderer_t* mode7_renderer_new(texture_t* render_texture);

/**
 * Frees a renderer.
 *
 * @param renderer Renderer to free.
 */
void mode7_renderer_free(mode7_renderer_t* renderer);

/**
 * Clears color buffer with given color.
 *
 * @param renderer Renderer to clear color buffer.
 * @param color Clear color.
 */
void mode7_renderer_clear_color(mode7_renderer_t* renderer, color_t color);

/**
 * Render given texture and horizontal scanline callback. Callback will be
 * invoked at the start of each horizontal scanline in the render texture.
 *
 * @param renderer Renderer to render to.
 * @param texture Texture to render.
 * @param callback Function to call at the start of each scanline.
 */
void mode7_renderer_render(mode7_renderer_t* renderer, texture_t* texture, mode7_callback_t callback);

typedef struct {
    mfloat_t position[VEC3_SIZE];

    float pitch;
    float yaw;

    float fov;

    float near;
    float far;

    mode7_renderer_t* renderer;
} mode7_camera_t;

/**
 * Creates a new camera.
 *
 * @return mode7_camera_t* Newly created renderer.
 */
mode7_camera_t* mode7_camera_new(mode7_renderer_t* renderer);

/**
 * Frees a camera.
 *
 * @param camera Camera to free.
 */
void mode7_camera_free(mode7_camera_t* camera);

/**
 * Callback function. Renders the camera view on a per scanline basis.
 *
 * @param camera Camera to render.
 * @param scanline Current scanline being rendered.
 */
void mode7_camera_call(mode7_camera_t* camera, int scanline);

#endif
