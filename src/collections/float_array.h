#ifndef FLOAT_ARRAY_H
#define FLOAT_ARRAY_H

#include <stdint.h>

typedef struct {
    size_t size;
    float* data;
} float_array_t;

float_array_t* float_array_new(size_t size);

void float_array_free(float_array_t* array);

float float_array_get(float_array_t* array, int index);

void float_array_set(float_array_t* array, int index, float value);

void float_array_add(float_array_t* array, float value);

#endif
