#ifndef STATE_H
#define STATE_H

#include <stdbool.h>

#include "event.h"

typedef struct {
    void (*init)(void);
    bool (*handle_event)(event_t* event);
    void (*update)(void);
    void (*draw)(void);
} state_t;

#endif
