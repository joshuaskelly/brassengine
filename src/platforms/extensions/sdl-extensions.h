#ifndef SDL_EXTENSIONS_H
#define SDL_EXTENSIONS_H

#include <SDL.h>

/**
 * Gets the scale of a window's pixel.
 *
 * @param window The window to query
 * @return Scale as a ratio of logical to pixel units.
 */
float SDL_GetWindowPixelScale(SDL_Window* window) {
    int sw, pw;
    SDL_GetWindowSize(window, &sw, NULL);
    SDL_GetWindowSizeInPixels(window, &pw, NULL);

    return (float)sw / pw;
}

/**
 * Sets the size of a window's client area in pixels.
 *
 * @param window The window to change
 * @param width Width of window in pixels
 * @param height Height of window in pixels
 */
void SDL_SetWindowSizeInPixels(SDL_Window* window, int width, int height) {
    float pixel_scale = SDL_GetWindowPixelScale(window);

    SDL_SetWindowSize(window, width * pixel_scale, height * pixel_scale);
}

#endif
