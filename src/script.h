#ifndef SCRIPT_H
#define SCRIPT_H

#include <stdbool.h>

enum b {
    SCRIPT_BUTTON_LEFT,
    SCRIPT_BUTTON_RIGHT,
    SCRIPT_BUTTON_UP,
    SCRIPT_BUTTON_DOWN,
    SCRIPT_BUTTON_A,
    SCRIPT_BUTTON_B,
};

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
void script_reload(void);

#endif
