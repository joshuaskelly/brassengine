#ifndef CIRCULAR_BUFFER
#define CIRCULAR_BUFFER

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    int head;
    int tail;
    size_t count;
    size_t max_count;
    void* data[];
} circular_buffer_t;

circular_buffer_t* circular_buffer_new(size_t count);

void circular_buffer_clear(circular_buffer_t* buffer);

void circular_buffer_add(circular_buffer_t* buffer, void* item);

void* circular_buffer_get(circular_buffer_t* buffer, int index);

#endif
