#include <stdlib.h>
#include <string.h>

#include "int_array.h"
#include "../log.h"

int_array_t* int_array_new(size_t size) {
    int_array_t* array = (int_array_t*)malloc(sizeof(int_array_t));
    array->size = size;
    array->data = (int*)malloc(sizeof(int) * size);

    memset(array->data, 0, sizeof(int) * size);

    return array;
}

void int_array_free(int_array_t* array) {
    free(array->data);
    array->data = NULL;
    free(array);
    array = NULL;
}

int int_array_get(int_array_t* array, int index) {
    return array->data[index];
}

void int_array_set(int_array_t* array, int index, int value) {
    array->data[index] = value;
}

void int_array_add(int_array_t* array, int value) {
    int* a = (int*)realloc(array->data, sizeof(int) * (array->size + 1));

    if (a != NULL) {
        array->data = a;
        array->data[array->size] = value;
        array->size += 1;
    }
    else {
        log_error("Failed to add value to int array");
    }
}

void int_array_resize(int_array_t* array, size_t size) {
    int* a = (int*)realloc(array->data, sizeof(int) * size);

    // Realloc to zero is implementation defined. So if we resize to zero and
    // get back a NULL pointer, that is okay.
    if (a != NULL || size == 0) {
        array->data = a;

        for (int i = array->size; i < size; i++) {
            array->data[i] = 0.0f;
        }

        array->size = size;
    }
    else {
        log_error("Failed to resize int array");
    }
}
