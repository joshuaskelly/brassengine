#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "assets.h"
#include "log.h"

static archive_t archive;

void assets_init(void) {
    archive.version = -1;
    archive.script = NULL;
    archive.textures = NULL;
    archive.texture_count = 0;

    if (!assets_load_archive("assets/test.toy")) {
        log_fatal("Failed to load archive.");
    }
}

void assets_destroy(void) {

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
    int i = 0;
    long position = ftell(fp);

    while (fgets(line, 1024, fp)) {
        if (sscanf(line, "%i %i %i", &r, &g, &b)) {
            uint32_t color = a << 24 | b << 16 | g << 8 | r;
            archive->palette[i++] = color;
            position = ftell(fp);
        }
        else {
            break;
        }
    }

    fseek(fp, position, SEEK_SET);
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

    char* script = (char*)malloc(sizeof(char) * string_size + 1);

    long char_left = string_size;

    while (fgets(line, char_left, fp)) {
        size_t length = strlen(line);

        if (length == 0) break;

        char_left -= length;
        strcat(script, line);
    }

    script[string_size] = '\0';

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

    for (int i = 0; i <  archive->texture_count; i++) {
        texture_t* texture = read_texture(fp);

        if (texture) {
            textures[i] = texture;
        }
    }

    archive->textures = textures;
}

void assets_unload_archive(void) {
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

bool assets_load_archive(char* filename) {
    FILE* fp = fopen(filename, "r");

    if (!fp) {
        fclose(fp);
        return false;
    }

    assets_unload_archive();

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

char* assets_get_script(void) {
    return archive.script;
}

uint32_t* assets_get_palette(void) {
    return archive.palette;
}

texture_t* assets_get_texture(int index) {
    texture_t* texture = archive.textures[index];
    return texture;
}
