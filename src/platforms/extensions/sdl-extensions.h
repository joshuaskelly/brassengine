#ifndef SDL_EXTENSIONS_H
#define SDL_EXTENSIONS_H

#include <SDL.h>

float SDL_GetWindowPixelScale(SDL_Window* window) {
    int sw, pw;
    SDL_GetWindowSize(window, &sw, NULL);
    SDL_GetWindowSizeInPixels(window, &pw, NULL);

    return (float)sw / pw;
}

void SDL_SetWindowSizeInPixels(SDL_Window* window, int w, int h) {
    float pixel_scale = SDL_GetWindowPixelScale(window);

    SDL_SetWindowSize(window, w * pixel_scale, h * pixel_scale);
}

#endif
