#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdbool.h>

#include "event.h"

void console_init(void);
bool console_handle_event(event_t* event);
void console_update(void);
void console_draw(void);

#endif