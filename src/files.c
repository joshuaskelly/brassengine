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

const char* files_read(const char* filename) {
    FILE* file = files_open_file(filename, "rb");
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

    fclose(file);

    return data;
}

static FILE* open_zip_entry_as_file(const char* filename, const char* mode) {
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

FILE* files_open_file(const char* filename, const char* mode) {
    if (files_check_extension(arguments_last(), "zip")) {
        return open_zip_entry_as_file(filename, mode);
    }

    char asset_path[1024];
    snprintf(
        asset_path,
        sizeof(asset_path),
        "%s/%s",
        assets_directory,
        filename
    );

    FILE* fp = fopen(asset_path, mode);

    if (!fp) {
        log_error("Failed to open file %s: %s", filename, strerror(errno));
        return NULL;
    }

    return fp;
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
