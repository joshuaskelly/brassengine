#ifndef CIRCULAR_BUFFER
#define CIRCULAR_BUFFER

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    /** Index of first element. */
    int head;

    /** Index of last element.*/
    int tail;

    /** Number of items in buffer. */
    size_t count;

    /** Maximum number of items buffer can contain. */
    size_t capacity;

    /** Buffer items. */
    void* data[];
} circular_buffer_t;

/**
 * Creates a circular buffer.
 *
 * @param capacity Maximum number of elements
 * @return circular_buffer_t* New circular buffer
 */
circular_buffer_t* circular_buffer_new(size_t capacity);

/**
 * Frees a circular buffer.
 *
 * @param buffer Circular buffer to free.
 */
void circular_buffer_free(circular_buffer_t* buffer);

/**
 * Removes all items from circular buffer.
 *
 * @param buffer Circular buffer to clear.
 */
void circular_buffer_clear(circular_buffer_t* buffer);

/**
 * Adds an item to a circular buffer.
 *
 * @param buffer Circular buffer to add to.
 * @param item Item to add to circular buffer.
 */
void circular_buffer_add(circular_buffer_t* buffer, void* item);

/**
 * Gets an item from a circular buffer.
 *
 * @param buffer Circular buffer to get item from.
 * @param index Index of item to get.
 * @return void* Item from circular buffer.
 */
void* circular_buffer_get(circular_buffer_t* buffer, int index);

#endif
