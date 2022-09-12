#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <giflib/gif_lib.h>

#include "arguments.h"
#include "assets.h"
#include "log.h"

static texture_t* texture = NULL;;

void assets_init(void) {
    log_info("assets init");

    assets_load();
}

void assets_destroy(void) {
    assets_unload();
}

bool assets_load(void) {
    // Load palette
    gif_t* palette = assets_gif_load("assets/palette.gif");
    graphics_palette_set(palette->palette);
    assets_gif_free(palette);

    // Load main texture
    gif_t* textures = assets_gif_load("assets/textures.gif");
    texture = texture_copy(textures->frames[0]);
    assets_gif_free(textures);

    return true;
}

void assets_unload(void) {
    texture_free(texture);
}

void assets_reload(void)  {
    assets_unload();
    assets_load();
}

texture_t* assets_get_texture(int index) {
    return texture;
}

gif_t* assets_gif_load(const char* filename) {
    int error;

    GifFileType* gif_file = DGifOpenFileName(filename, &error);

    // Open the GIF file
    if (!gif_file) {
        log_error("Failed to open: %s", filename);
        return NULL;
    }

    // Read GIF file contents
    if (DGifSlurp(gif_file) == GIF_ERROR) {
        log_error("Failed to read GIF data");
        DGifCloseFile(gif_file, &error);
        return NULL;
    }

    // Get palette
    ColorMapObject* color_map = gif_file->SColorMap;

    if (!color_map) {
        log_error("GIF missing common color map");
        DGifCloseFile(gif_file, &error);
        return NULL;
    }

    uint32_t* palette = (uint32_t*)calloc(256, sizeof(uint32_t));

    if (!palette) {
        log_error("Failed to create palette for GIF");
        DGifCloseFile(gif_file, &error);
        return NULL;
    }

    for (int i = 0; i < color_map->ColorCount; i++) {
        GifColorType gct = color_map->Colors[i];

        int r = gct.Red;
        int g = gct.Green;
        int b = gct.Blue;
        int a = 0xFF;

        uint32_t color = a << 24 | b << 16 | g << 8 | r;

        palette[i] = color;
    }

    // Get frames
    int frame_count = gif_file->ImageCount;
    texture_t** textures = (texture_t**)malloc(sizeof(texture_t*) * frame_count);

    if (!textures) {
        log_error("Failed to create frames for GIF");
        DGifCloseFile(gif_file, &error);
        return NULL;
    }

    for (int i = 0; i < frame_count; i++) {
        SavedImage saved_image = gif_file->SavedImages[i];
        GifImageDesc image_desc = saved_image.ImageDesc;

        texture_t* t = texture_new(image_desc.Width, image_desc.Height, saved_image.RasterBits);
        textures[i] = t;
    }

    DGifCloseFile(gif_file, &error);

    // Create gif_t
    gif_t* gif = (gif_t*)malloc(sizeof(gif_t));

    if (!gif) {
        log_error("Failed to create GIF");
        return NULL;
    }

    gif->frame_count = frame_count;
    gif->frames = textures;
    gif->palette = palette;

    return gif;
}

void assets_gif_free(gif_t* gif) {
    for (int i = 0; i < gif->frame_count; i++) {
        free(gif->frames[i]);
    }

    free(gif->frames);
    free(gif->palette);
    free(gif);
}
