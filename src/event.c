#include <stdbool.h>

#include "event.h"

static event_t queue[MAXEVENTS];
static int head = 0;
static int tail = 0;

void event_post(event_t* event) {
    queue[head] = *event;
    head = (head + 1) % MAXEVENTS;
}

bool event_poll(event_t* event) {
    if (tail == head) return false;

    *event = queue[tail];
    tail = (tail + 1) % MAXEVENTS;

    return true;
}
