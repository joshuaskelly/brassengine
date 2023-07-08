/**
 * @file int_array.h
 * int array implementation.
 */

#ifndef INT_ARRAY_H
#define INT_ARRAY_H

#include <stdint.h>

typedef struct {
    size_t size;
    int* data;
} int_array_t;

/**
 * Creates a new int array of given size.
 *
 * @param size Number of int elements.
 * @return int_array_t*
 */
int_array_t* int_array_new(size_t size);

/**
 * Frees a int array.
 *
 * @param array int array to free.
 */
void int_array_free(int_array_t* array);

/**
 * Get value for indexed element.
 *
 * @param array int array to access.
 * @param index Element index.
 * @return int Value at given index.
 */
int int_array_get(int_array_t* array, int index);

/**
 * Set indexed element to given value.
 *
 * @param array int array to modify.
 * @param index Element index.
 * @param value New value
 */
void int_array_set(int_array_t* array, int index, int value);

/**
 * Adds new element with given value to end of int array.
 *
 * @param array int array to modify.
 * @param value New value
 */
void int_array_add(int_array_t* array, int value);

/**
 * Resize int array to given length.
 *
 * @param array int array to modify.
 * @param size New number of elements.
 */
void int_array_resize(int_array_t* array, size_t size);

#endif
