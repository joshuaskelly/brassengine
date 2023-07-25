#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#include <zip/zip.h>

#include "assets.h"
#include "arguments.h"
#include "files.h"
#include "log.h"

static FILE* get_from_zip(const char* filename);
static FILE* get_from_assets_directory(const char* filename);

const char* files_read(const char* filename) {
    FILE* file = files_get(filename);
    if (!file) {
        return NULL;
    }

    // Get data size
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    // Allocate memory
    char* data = calloc(size + 1, sizeof(char));
    if (!data) {
        log_error("Failed to allocate memory for file_read.");
        fclose(file);
        return NULL;
    }

    // Read data in bytes
    size_t ret_code = fread(data, 1, size, file);
    if (ret_code != size) {
        if (feof(file)) {
            log_error("Error reading %s: unexpected end of file\n", filename);
        }
        else if (ferror(file)) {
            log_error("Error reading %s", filename);
        }
    }

    return data;
}

FILE* files_get(const char* filename) {
    // Check if user gave us a zip file or asset directory
    if (arguments_count() > 1) {
        const char* zip_or_directory = arguments_last();

        // If we are given a zip file, load it
        if (files_check_extension(zip_or_directory, "zip")) {
            return get_from_zip(filename);
        }
    }

    return get_from_assets_directory(filename);
}

static FILE* get_from_zip(const char* filename) {
    errno = 0;

    // Get a temp file to write to
    FILE* temp_file = tmpfile();
    if (!temp_file) {
        log_error("Failed to open temp file: %s", strerror(errno));
        return NULL;
    }

    struct zip_t* zip = zip_open(arguments_last(), 0, 'r');
    int total_zip_entries = zip_entries_total(zip);
    for (int i = 0; i < total_zip_entries; i++) {
        zip_entry_openbyindex(zip, i);

        // Early out on directories
        if (zip_entry_isdir(zip)) {
            zip_entry_close(zip);
            continue;
        }

        // Find the zip entry
        const char* name = zip_entry_name(zip);
        if (strcmp(filename, name) != 0) {
            zip_entry_close(zip);
            continue;
        }

        // Read zip entry data
        size_t buffer_size = zip_entry_size(zip);
        uint8_t* buffer = calloc(buffer_size, sizeof(uint8_t));
        zip_entry_noallocread(zip, (void*)buffer, buffer_size);
        zip_entry_close(zip);

        // Write data to temp file
        fwrite(buffer, sizeof(uint8_t), buffer_size, temp_file);
        rewind(temp_file);

        free(buffer);

        break;
    }

    zip_close(zip);

    return temp_file;
}

static FILE* get_from_assets_directory(const char* filename) {
    char asset_path[1024];
    snprintf(
        asset_path,
        sizeof(asset_path),
        "%s/%s",
        assets_directory,
        filename
    );

    return fopen(asset_path, "rb");
}

bool files_check_extension(const char* filename, const char* ext) {
    if (filename == NULL || ext == NULL) return false;

    const char* dot = strrchr(filename, '.');

    if (!dot) return false;

    return strcmp(dot + 1, ext) == 0;
}

void files_walk_directory(char* directory, void(callback)(const char*)) {
    DIR* dir = opendir(directory);
    if (!dir) return;

    struct dirent* entry;
    struct stat s;
    char fullpath[512];
    memset(fullpath, 0, 512);

    // Iterate over directory contents
    while ((entry = readdir(dir))) {
        // Ignore current directory and parent directory
        if (!strcmp(entry->d_name, ".\0")) continue;
        if (!strcmp(entry->d_name, "..\0")) continue;

        // Update fullpath
        snprintf(fullpath, sizeof(fullpath), "%s/%s", directory, entry->d_name);
        stat(fullpath, &s);

        if (S_ISDIR(s.st_mode)) {
            files_walk_directory(fullpath, callback);
        }
        else {
            callback(fullpath);
        }
    }

    closedir(dir);
}
