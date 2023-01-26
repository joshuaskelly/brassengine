#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "circular_buffer.h"

void circular_buffer_clear(circular_buffer_t* buffer) {
    buffer->head = 0;
    buffer->tail = 0;
    buffer->count = 0;

    for (int i = 0; i < buffer->max_count; i++) {
        if (buffer->data[i]) {
            free(buffer->data[i]);
        }
        buffer->data[i] = NULL;
    }
}

circular_buffer_t* circular_buffer_new(size_t count) {
    circular_buffer_t* buffer = malloc(sizeof(int) * 2 + sizeof(size_t) * 2 + sizeof(void*) * count);
    buffer->max_count = count;
    memset(buffer->data, 0, sizeof(void*) * count);
    circular_buffer_clear(buffer);

    return buffer;
}

void circular_buffer_add(circular_buffer_t* buffer, void* item) {
    buffer->data[buffer->tail] = item;

    if (buffer->count < buffer->max_count - 1) {
        buffer->count++;
    }

    int next = (buffer->tail + 1) % buffer->max_count;
    if (next == buffer->head) {
        free(buffer->data[buffer->head]);
        buffer->data[buffer->head] = NULL;
        buffer->head = (buffer->head + 1) % buffer->max_count;
    }

    buffer->tail = next;
}

void* circular_buffer_get(circular_buffer_t* buffer, int index) {
    int i = (buffer->head + index) % buffer->max_count;
    return buffer->data[i];
}
