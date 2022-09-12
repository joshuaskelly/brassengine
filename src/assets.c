#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <giflib/gif_lib.h>

#include "arguments.h"
#include "assets.h"
#include "log.h"

static archive_t archive;
static char* archive_filename;

void set_archive_filename(const char* filename);

void assets_init(void) {
    log_info("assets init");

    archive.version = -1;
    archive.script = NULL;
    archive.textures = NULL;
    archive.texture_count = 0;

    int argc = arguments_count();
    if (argc > 1) {
        set_archive_filename(arguments_vector()[argc - 1]);
    }
    else {
        set_archive_filename("assets/default.toy");
    }

    if (!assets_load(archive_filename)) {
        log_fatal("Failed to load archive.");
    }
}

void assets_destroy(void) {
    assets_unload();
}

void read_header(FILE* fp, archive_t* archive) {
    char line[1024];
    if (fgets(line, 1024, fp)) {
        sscanf(line, "version %i", &archive->version);
    };
}

void read_palette(FILE* fp, archive_t* archive) {
    for (int i = 0; i < 256; i++) {
        archive->palette[i] = 0;
    }

    int r = 0;
    int g = 0;
    int b = 0;
    int a = 0xFF;

    char line[1024];
    int palette_color_count = 0;
    long position = ftell(fp);

    while (fgets(line, 1024, fp)) {
        if (sscanf(line, "%i %i %i", &r, &g, &b)) {
            uint32_t color = a << 24 | b << 16 | g << 8 | r;
            archive->palette[palette_color_count++] = color;
            position = ftell(fp);
        }
        else {
            break;
        }
    }

    fseek(fp, position, SEEK_SET);

    graphics_palette_clear();
    uint32_t* palette = graphics_palette_get();

    for (int i = 0; i < palette_color_count; i++) {
        palette[i] = archive->palette[i];
    }
}

const char* tag_pattern = "__%[abcdefghijklmnopqrstuvwxyz]__";

void read_script(FILE* fp, archive_t* archive) {
    long position = ftell(fp);
    long string_size = 0;

    char line[1024];
    char tag[32];

    while (fgets(line, 1024, fp)) {
        if (sscanf(line, tag_pattern, &tag)) {
            break;
        }
        string_size += strlen(line);
    }

    fseek(fp, position, SEEK_SET);

    char* script = (char*)calloc(string_size + 1, sizeof(char));

    if (script == NULL) {
        log_fatal("Unable to allocate memory for script.");
    }

    long char_left = string_size;

    while (fgets(line, char_left, fp)) {
        size_t length = strlen(line);

        if (length == 0) break;

        char_left -= length;
        strcat(script, line);
    }

    archive->script = script;
}

int get_texture_count(FILE* fp) {
    int count = 0;

    char line[1024];
    while (fgets(line, 1024, fp)) {
        if (strncmp(line, "__texture__", 11) == 0) {
            count++;
        }
    }

    rewind(fp);

    return count;
}

texture_t* read_texture(FILE* fp) {
    char line[1024];
    fgets(line, 1024, fp);

    if (strncmp(line, "__texture__", 11) != 0) return NULL;

    int width = 0;
    int height = 0;

    fgets(line, 1024, fp);
    if (!sscanf(line, "%i %i", &width, &height)) return NULL;

    char* end = NULL;

    color_t pixels[width * height];

    int i = 0;
    while (fgets(line, 1024, fp)) {
        pixels[i++] = strtol(line, &end, 10);

        while (end && strncmp(end,"\n", 1)) {
            pixels[i++] = strtol(end, &end, 10);
        }
    }

    return texture_new(width, height, pixels);
}

void read_textures(FILE* fp, archive_t* archive) {
    long position = ftell(fp);

    char line[1024];
    while (fgets(line, 1024, fp)) {
        if (strncmp(line, "__texture__", 11) == 0) {
            archive->texture_count++;
        }
    }

    fseek(fp, position, SEEK_SET);

    texture_t** textures = (texture_t**)malloc(sizeof(texture_t) * archive->texture_count);

    if (textures == NULL) {
        log_fatal("Unable to allocate memory for texture array.");
    }

    for (int i = 0; i <  archive->texture_count; i++) {
        texture_t* texture = read_texture(fp);

        if (texture) {
            textures[i] = texture;
        }
    }

    archive->textures = textures;
}

void set_archive_filename(const char* filename) {
    char* old_filename = archive_filename;
    archive_filename = (char*)calloc(strlen(filename) + 1, sizeof(char));
    strcpy(archive_filename, filename);
    free(old_filename);
}

bool assets_load(const char* filename) {
    if (!assets_is_archive_file(filename)) {
        log_error("%s is not a valid archive file.");
        return false;
    }

    FILE* fp = fopen(filename, "r");

    if (!fp) {
        log_error("Failed to open: %s", filename);
        fclose(fp);
        return false;
    }

    log_info("loaded file: %s", filename);

    set_archive_filename(filename);

    assets_unload();

    char line[1024];
    while (fgets(line, 1024, fp)) {
        if (strncmp(line, "__header__", 10) == 0) {
            read_header(fp, &archive);
        }
        else if (strncmp(line, "__palette__", 11) == 0) {
            read_palette(fp, &archive);
        }
        else if (strncmp(line, "__script__", 10) == 0) {
            read_script(fp, &archive);
        }
        else if (strncmp(line, "__textures__", 12) == 0) {
            read_textures(fp, &archive);
        }
    }

    fclose(fp);

    return true;
}

void assets_unload(void) {
    if (archive.version == -1) return;

    archive.version = -1;
    free(archive.script);
    archive.script = NULL;

    for (int i = 0; i < archive.texture_count; i++) {
        texture_t* texture = archive.textures[i];
        texture_free(texture);
        texture = NULL;
    }

    free(archive.textures);
    archive.textures = NULL;

    archive.texture_count = 0;
}

void assets_reload(void)  {
    assets_load(archive_filename);
}

char* assets_get_script(void) {
    return archive.script;
}

uint32_t* assets_get_palette(void) {
    return archive.palette;
}

texture_t* assets_get_texture(int index) {
    if (archive.textures == NULL) return NULL;

    texture_t* texture = archive.textures[index];
    return texture;
}

bool assets_is_archive_file(const char* filename) {
    const char* dot = strrchr(filename, '.');
    return dot && strcmp(dot, ".toy") == 0;
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
