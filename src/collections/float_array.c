#include <stdlib.h>
#include <string.h>

#include "float_array.h"
#include "../log.h"

float_array_t* float_array_new(size_t size) {
    float_array_t* array = (float_array_t*)malloc(sizeof(float_array_t));
    array->size = size;
    array->data = (float*)malloc(sizeof(float) * size);

    memset(array->data, 0, sizeof(float) * size);

    return array;
}

void float_array_free(float_array_t* array) {
    free(array);
    array = NULL;
}

float float_array_get(float_array_t* array, int index) {
    return array->data[index];
}

void float_array_set(float_array_t* array, int index, float value) {
    array->data[index] = value;
}

void float_array_add(float_array_t* array, float value) {
    float* a = (float*)realloc(array->data, sizeof(float) * (array->size + 1));

    if (a != NULL) {
        array->data = a;
        array->data[array->size] = value;
        array->size += 1;
    }
    else {
        log_error("Failed to add value to float array");
    }
}

void float_array_resize(float_array_t* array, size_t size) {
    float* a = (float*)realloc(array->data, sizeof(float) * size);

    if (a != NULL || size == 0) {
        array->data = a;

        for (int i = array->size; i < size; i++) {
            array->data[i] = 0.0f;
        }

        array->size = size;
    }
    else {
        log_error("Failed to resize float array");
    }
}
