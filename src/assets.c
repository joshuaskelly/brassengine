#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <giflib/gif_lib.h>
#include <zip/zip.h>

#include "arguments.h"
#include "assets.h"
#include "log.h"

static texture_t* texture = NULL;
static char* script = NULL;

typedef struct {
    int frame_count;
    texture_t** frames;
    uint32_t* palette;
} gif_t;

gif_t* gif_load(const char* filename);
void gif_free(gif_t* gif);
gif_t* gif_load_from_buffer(void* buffer, size_t buffer_size);;

void assets_init(void) {
    log_info("assets init");

    if (!assets_load()) {
        log_fatal("assets init failed");
    }
}

void assets_destroy(void) {
    assets_unload();
}

/**
 * @brief Load assets from zip file.
 *
 * @return true if successful, false otherwise.
 */
bool load_from_zip(void) {
    const char* filename = arguments_last();
    struct zip_t* zip = zip_open(filename, 0, 'r');

    if (!zip) {
        log_error("Failed to open zip file: %s", filename);
        return false;
    }

    log_info("loading zip file: %s", filename);

    void* buffer = NULL;
    size_t buffer_size = 0;

    // Load palette
    zip_entry_open(zip, "palette.gif");
    buffer_size = zip_entry_size(zip);
    zip_entry_read(zip, &buffer, &buffer_size);
    zip_entry_close(zip);
    gif_t* palette = gif_load_from_buffer(buffer, buffer_size);
    graphics_palette_set(palette->palette);
    gif_free(palette);
    free(buffer);
    buffer = NULL;

    // Load textures
    zip_entry_open(zip, "textures.gif");
    buffer_size = zip_entry_size(zip);
    zip_entry_read(zip, &buffer, &buffer_size);
    zip_entry_close(zip);
    gif_t* textures = gif_load_from_buffer(buffer, buffer_size);
    texture = texture_copy(textures->frames[0]);
    gif_free(textures);
    free(buffer);
    buffer = NULL;

    // Load script
    zip_entry_open(zip, "main.lua");
    buffer_size = zip_entry_size(zip);
    zip_entry_read(zip, &buffer, &buffer_size);
    zip_entry_close(zip);
    script = calloc(buffer_size + 1, sizeof(char));
    memcpy(script, buffer, buffer_size);
    free(buffer);
    buffer = NULL;

    zip_close(zip);

    return true;
}

/**
 * @brief Load assets from assets directory.
 *
 * @return true If successful, false otherwise.
 */
bool load_from_assets_directory(void) {
    log_info("loading directory: %s", "assets");

    // Load palette
    gif_t* palette = gif_load("assets/palette.gif");

    if (!palette) {
        log_error("Failed to load palette file.");
        return false;
    }

    graphics_palette_set(palette->palette);
    gif_free(palette);

    // Load textures
    gif_t* textures = gif_load("assets/textures.gif");

    if (!textures) {
        log_error("Failed to load textures");
        return false;
    }

    texture = texture_copy(textures->frames[0]);
    gif_free(textures);

    // Load script
    FILE* fp = fopen("assets/main.lua", "r");
    if (!fp) {
        log_error("Failed to open script");
        return false;
    }

    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    script = calloc(size + 1, sizeof(char));

    if (!script) {
        log_error("Failed to allocate memory for script.");
        fclose(fp);
        return false;
    }

    fread(script, 1, size, fp);

    if (!script) {
        log_error("Failed to read script data.");
        fclose(fp);
        return false;
    }

    fclose(fp);

    return true;
}

bool assets_load(void) {
    if (arguments_count() > 1) {
        return load_from_zip();
    }

    return load_from_assets_directory();
}

void assets_unload(void) {
    texture_free(texture);
    texture = NULL;
    free(script);
    script = NULL;
}

void assets_reload(void)  {
    assets_unload();
    assets_load();
}

texture_t* assets_get_texture(int index) {
    return texture;
}

const char* assets_get_script(void) {
    return (const char*)script;
}

/**
 * @brief Processes GifFileType* into a gif_t*
 *
 * @param gif_file GifFileType* to process
 * @return gif_t* new gif_t if successful, NULL otherwise
 */
gif_t* load_gif_internal(GifFileType* gif_file) {
    int error;

    // Read GIF file contents
    if (DGifSlurp(gif_file) == GIF_ERROR) {
        log_error("Failed to read GIF data");
        DGifCloseFile(gif_file, &error);
        return NULL;
    }

    // Get common palette
    ColorMapObject* color_map = gif_file->SColorMap;

    if (!color_map) {
        log_error("GIF missing common color map");

        // Get palette from first image
        SavedImage first_image = gif_file->SavedImages[0];
        GifImageDesc first_desc = first_image.ImageDesc;
        color_map = first_desc.ColorMap;

        if (!color_map) {
            DGifCloseFile(gif_file, &error);
            return NULL;
        }

        log_error("Using color map from first frame.");
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

typedef struct {
    size_t size;
    size_t position;
    uint8_t* data;
} gif_read_buffer_t;

/**
 * @brief Callback function for DGifOpen(). Copies bytes from one buffer to
 * another.
 *
 * @param gif GIF being constructed.
 * @param dest Out byte buffer.
 * @param bytes_to_read Number of bytes to read.
 * @return int Number of bytes read.
 */
int read_data_from_buffer(GifFileType* gif, GifByteType* dest, int bytes_to_read) {
    gif_read_buffer_t* buffer;
    size_t bytes_read;

    // Grab our buffer struct
    buffer = (gif_read_buffer_t*)gif->UserData;

    if (!buffer) {
        log_error("UserData not set.");
        return -1;
    }

    // Check bounds
    if (buffer->position > buffer->size) {
        return -1;
    }

    // Determine how many bytes to read.
    if (buffer->position < buffer->size - bytes_to_read) {
        bytes_read = bytes_to_read;
    }
    else {
        bytes_read = buffer->size - buffer->position;
    }

    // Copy bytes to out buffer
    memcpy(dest, buffer->data + buffer->position, bytes_read);
    buffer->position += bytes_read;

    return bytes_read;
}

/**
 * @brief Load a GIF from a buffer of bytes.
 *
 * @param buffer byte buffer
 * @param buffer_size buffer size
 * @return gif_t* new gif_t if successful, NULL otherwise.
 */
gif_t* gif_load_from_buffer(void* buffer, size_t buffer_size) {
    gif_read_buffer_t buffer_;
    buffer_.data = (GifByteType*)buffer;
    buffer_.size = buffer_size;
    buffer_.position = 0;

    GifFileType* gif_file = DGifOpen(&buffer_, read_data_from_buffer, NULL);

    // Open the GIF file
    if (!gif_file) {
        log_error("Failed to open gif from memory");
        return NULL;
    }

    return load_gif_internal(gif_file);
}

/**
 * @brief Load a GIF from a file.
 *
 * @param filename GIF file path.
 * @return gif_t* new gif_t if successful, NULL otherwise.
 */
gif_t* gif_load(const char* filename) {
    int error;

    GifFileType* gif_file = DGifOpenFileName(filename, &error);

    // Open the GIF file
    if (!gif_file) {
        log_error("Failed to open: %s", filename);
        return NULL;
    }

    return load_gif_internal(gif_file);
}

/**
 * @brief Frees a gif_t.
 *
 * @param gif gif_t to free
 */
void gif_free(gif_t* gif) {
    for (int i = 0; i < gif->frame_count; i++) {
        free(gif->frames[i]);
        gif->frames[i] = NULL;
    }

    free(gif->frames);
    gif->frames = NULL;
    free(gif->palette);
    gif->palette = NULL;
    free(gif);
    gif = NULL;
}