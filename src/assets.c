#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <giflib/gif_lib.h>
#include <zip/zip.h>

#include "arguments.h"
#include "assets.h"
#include "graphics.h"
#include "log.h"

typedef struct {
    const char* name;
    void* asset;
} asset_entry_t;

/**
 * Create a new asset entry
 *
 * @param name Name of asset
 * @param asset Asset data
 * @return asset_entry_t
 */
asset_entry_t assets_entry_new(const char* name, void* asset) {
    int n = strlen(name);
    char* asset_name = (char*)calloc(n + 1, sizeof(char));
    strncpy(asset_name, name, n);

    return (asset_entry_t) {asset_name, asset};
}

static asset_entry_t* texture_assets = NULL;
static int texture_asset_count = 0;
static asset_entry_t* script_assets = NULL;
static int script_asset_count = 0;

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
 * Check filename extension.
 *
 * @param filename Filename to check
 * @param ext Extension to look for
 * @return true if match, false otherwise
 */
bool check_extension(const char* filename, const char* ext) {
    if (filename == NULL || ext == NULL) return false;

    const char* dot = strrchr(filename, '.');

    if (!dot) return false;

    int n = strlen(ext);

    return strncmp(dot + 1, ext, n) == 0;
}

/**
 * Load assets from zip file.
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

    int total_zip_entries = zip_entries_total(zip);

    // Load textures
    for (int i = 0; i < total_zip_entries; i++) {
        zip_entry_openbyindex(zip, i);

        if (zip_entry_isdir(zip)) {
            zip_entry_close(zip);
            continue;
        }

        const char* name = zip_entry_name(zip);

        if (check_extension(name, "gif")) texture_asset_count++;

        zip_entry_close(zip);
    }

    texture_assets = (asset_entry_t*)malloc(sizeof(asset_entry_t) * texture_asset_count);
    int j = 0;

    for (int i = 0; i < total_zip_entries; i++) {
        zip_entry_openbyindex(zip, i);

        if (zip_entry_isdir(zip)) {
            zip_entry_close(zip);
            continue;
        }

        const char* name = zip_entry_name(zip);

        if (check_extension(name, "gif")) {
            buffer_size = zip_entry_size(zip);
            zip_entry_read(zip, &buffer, &buffer_size);
            gif_t* textures = gif_load_from_buffer(buffer, buffer_size);
            texture_t* texture = graphics_texture_copy(textures->frames[0]);
            texture_assets[j++] = assets_entry_new(name, texture);
            zip_entry_close(zip);
            gif_free(textures);
            free(buffer);
            buffer = NULL;
        }

        zip_entry_close(zip);
    }

    // Load scripts
    for (int i = 0; i < total_zip_entries; i++) {
        zip_entry_openbyindex(zip, i);

        if (zip_entry_isdir(zip)) {
            zip_entry_close(zip);
            continue;
        }

        const char* name = zip_entry_name(zip);

        if (check_extension(name, "lua")) script_asset_count++;

        zip_entry_close(zip);
    }

    script_assets = (asset_entry_t*)malloc(sizeof(asset_entry_t) * script_asset_count);
    j = 0;

    for (int i = 0; i < total_zip_entries; i++) {
        zip_entry_openbyindex(zip, i);

        if (zip_entry_isdir(zip)) {
            zip_entry_close(zip);
            continue;
        }

        const char* name = zip_entry_name(zip);

        if (check_extension(name, "lua")) {
            buffer_size = zip_entry_size(zip);
            zip_entry_read(zip, &buffer, &buffer_size);
            char* script = calloc(buffer_size + 1, sizeof(char));
            memcpy(script, buffer, buffer_size);
            script_assets[j++] = assets_entry_new(name, script);
            zip_entry_close(zip);
            free(buffer);
            buffer = NULL;
        }

        zip_entry_close(zip);
    }

    zip_close(zip);

    return true;
}

/**
 * Load assets from assets directory.
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

    texture_t* texture = graphics_texture_copy(textures->frames[0]);

    // Create texture asset entries
    texture_asset_count = 1;
    texture_assets = (asset_entry_t*)malloc(sizeof(asset_entry_t) * texture_asset_count);
    texture_assets[0] = assets_entry_new("textures.gif", texture);

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

    char* script = calloc(size + 1, sizeof(char));

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

    // Create script asset entries
    script_asset_count = 1;
    script_assets = (asset_entry_t*)malloc(sizeof(asset_entry_t) * script_asset_count);
    script_assets[0] = assets_entry_new("main.lua", script);

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
    // Free textures
    for (int i = 0; i < texture_asset_count; i++) {
        graphics_texture_free(texture_assets[i].asset);
        free((char*)texture_assets[i].name);
        texture_assets[i].name = NULL;
    }
    free(texture_assets);
    texture_assets = NULL;
    texture_asset_count = 0;

    // Free scripts
    for (int i = 0; i < script_asset_count; i++) {
        free((char *)script_assets[i].asset);
        script_assets[i].asset = NULL;
        free((char *)script_assets[i].name);
        script_assets[i].name = NULL;
    }
    free(script_assets);
    script_assets = NULL;
    script_asset_count = 0;
}

void assets_reload(void)  {
    assets_unload();
    assets_load();
}

/**
 * Get asset for given name.
 *
 * @param assets Array of asset_entry_t
 * @param count Count of asset_entry_t array
 * @param name Name to look for
 * @return void* Asset for name if found, NULL otherwise
 */
void* asset_get(asset_entry_t* assets, int count, const char* name) {
    for (int i = 0; i < count; i++) {
        const char* asset_name = assets[i].name;
        if (strcmp(name, asset_name) == 0) {
            return assets[i].asset;
        }
    }

    return NULL;
}

texture_t* assets_get_texture(const char* filename) {
    return (texture_t*)asset_get(texture_assets, texture_asset_count, filename);
}

const char* assets_get_script(const char* filename) {
    return(const char*)asset_get(script_assets, script_asset_count, filename);
}

/**
 * Processes GifFileType* into a gif_t*
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

        texture_t* t = graphics_texture_new(image_desc.Width, image_desc.Height, saved_image.RasterBits);
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
 * Callback function for DGifOpen(). Copies bytes from one buffer to
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
 * Load a GIF from a buffer of bytes.
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
 * Load a GIF from a file.
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
 * Frees a gif_t.
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
