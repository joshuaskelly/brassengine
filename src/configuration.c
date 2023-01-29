#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cjson/cJSON.h>

#include "arguments.h"
#include "configuration.h"
#include "log.h"

struct config* config = NULL;

static void defaults_set(struct config* config) {
    config->resolution.width = 320;
    config->resolution.height = 200;
    config->console.colors.foreground = 1;
    config->console.colors.background = 0;
    config->console.colors.cursor = 1;

    char* prompt = (char*)malloc(sizeof(char) * 3);
    char* default_prompt = "> ";
    strncpy(prompt, default_prompt, 3);

    config->console.prompt = prompt;
}

static bool check_extension(const char* filename, const char* ext) {
    if (filename == NULL || ext == NULL) return false;

    const char* dot = strrchr(filename, '.');

    if (!dot) return false;

    int n = strlen(ext);

    return strncmp(dot + 1, ext, n) == 0;
}

static void set(cJSON* json) {
    cJSON* resolution = cJSON_GetObjectItemCaseSensitive(json, "resolution");
    if (resolution) {
        cJSON* width = cJSON_GetObjectItemCaseSensitive(resolution, "width");
        cJSON* height = cJSON_GetObjectItemCaseSensitive(resolution, "height");

        if (cJSON_IsNumber(width) && cJSON_IsNumber(height)) {
            config->resolution.width = width->valueint;
            config->resolution.height = height->valueint;
        }
    }

    cJSON* console = cJSON_GetObjectItemCaseSensitive(json, "console");
    if (console) {
        cJSON* colors = cJSON_GetObjectItemCaseSensitive(console, "colors");
        if (colors) {
            cJSON* foreground = cJSON_GetObjectItemCaseSensitive(colors, "foreground");
            cJSON* background = cJSON_GetObjectItemCaseSensitive(colors, "background");
            cJSON* cursor = cJSON_GetObjectItemCaseSensitive(colors, "cursor");

            if (cJSON_IsNumber(foreground)) {
                config->console.colors.foreground = foreground->valueint;
            }

            if (cJSON_IsNumber(background)) {
                config->console.colors.background = background->valueint;
            }

            if (cJSON_IsNumber(cursor)) {
                config->console.colors.cursor = cursor->valueint;
            }
        }

        cJSON* prompt = cJSON_GetObjectItemCaseSensitive(console, "prompt");
        if (prompt && cJSON_IsString(prompt)) {
            free(config->console.prompt);

            char* s = prompt->valuestring;
            char* prompt = (char*)malloc(sizeof(char) * (strlen(s) + 1));
            strncpy(prompt, s, strlen(s));
            prompt[strlen(s)] = '\0';
            config->console.prompt = prompt;
        }
    }
}

static void init_from_assets_directory(const char* directory) {
    // Load palette
    char config_path[strlen(directory) + 13];
    memset(config_path, 0, strlen(directory) + 13);
    strcat(config_path, directory);
    strcat(config_path, "/config.json");

    // Open configuration file
    FILE* fp = fopen(config_path, "rb");
    if (!fp) {
        return;
    }

    // Get configuration size
    fseek(fp, 0, SEEK_END);
    size_t size = ftell(fp);
    rewind(fp);

    // Allocate memory
    char* data = calloc(size + 1, sizeof(char));
    if (!data) {
        log_error("Failed to allocate memory for script.");
        fclose(fp);
        return;
    }

    // Read in configuration bytes
    size_t ret_code = fread(data, 1, size, fp);
    if (ret_code != size) {
        if (feof(fp)) {
            log_error("Error reading %s: unexpected end of file\n", config_path);
        }
        else if (ferror(fp)) {
            log_error("Error reading %s", config_path);
        }
    }

    cJSON* json = cJSON_Parse(data);

    set(json);

    cJSON_Delete(json);

    fclose(fp);
}

static void init_from_zip(const char* zip) {

}

void configuration_init(void) {
    config = (struct config*)malloc(sizeof(struct config));
    defaults_set(config);

    // Check if user gave us a zip file or asset directory
    if (arguments_count() > 1) {
        const char* zip_or_directory = arguments_last();

        // If we are given a zip file, load it
        if (check_extension(zip_or_directory, "zip")) {
            init_from_zip(zip_or_directory);
            return;
        }

        init_from_assets_directory(zip_or_directory);
        return;
    }

    init_from_assets_directory("assets");
}


void configuration_destroy(void) {
    free(config->console.prompt);
    free(config);
}
