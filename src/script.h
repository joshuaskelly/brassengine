#ifndef SCRIPT_H
#define SCRIPT_H

#include <stdbool.h>

/**
 * @brief Initialize scripting system.
 */
void script_init(void);

/**
 * @brief Destroy scripting system.
 */
void script_destroy(void);

bool script_handle_event(event_t* event);
void script_update(void);
void script_setup(void);
void script_draw(void);

#endif
