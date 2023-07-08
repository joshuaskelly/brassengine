/**
 * @file float_array.h
 * Float array implementation.
 */

#ifndef FLOAT_ARRAY_H
#define FLOAT_ARRAY_H

#include <stdint.h>

typedef struct {
    size_t size;
    float* data;
} float_array_t;

/**
 * Creates a new float array of given size.
 *
 * @param size Number of float elements.
 * @return float_array_t*
 */
float_array_t* float_array_new(size_t size);

/**
 * Frees a float array.
 *
 * @param array Float array to free.
 */
void float_array_free(float_array_t* array);

/**
 * Get value for indexed element.
 *
 * @param array Float array to access.
 * @param index Element index.
 * @return float Value at given index.
 */
float float_array_get(float_array_t* array, int index);

/**
 * Set indexed element to given value.
 *
 * @param array Float array to modify.
 * @param index Element index.
 * @param value New value
 */
void float_array_set(float_array_t* array, int index, float value);

/**
 * Adds new element with given value to end of float array.
 *
 * @param array Float array to modify.
 * @param value New value
 */
void float_array_add(float_array_t* array, float value);

/**
 * Resize float array to given length.
 *
 * @param array Float array to modify.
 * @param size New number of elements.
 */
void float_array_resize(float_array_t* array, size_t size);

#endif
