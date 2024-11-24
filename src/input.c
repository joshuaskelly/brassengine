#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "event.h"
#include "input.h"

static uint8_t keyboard_codes[KEYCODE_NUM_CODES];
static uint8_t keyboard_symbols[KEYSYMBOL_NUM_SYMBOLS];

static struct {
    int x;
    int y;
    int motion_x;
    int motion_y;
    int wheel_x;
    int wheel_y;
    uint32_t buttons;
} mouse_state;

typedef struct controller_state {
    uint8_t id;
    uint8_t buttons[CONTROLLER_BUTTON_COUNT];
    float axis[CONTROLLER_AXIS_COUNT];
    struct controller_state* next;
} controller_state_t;

static controller_state_t* controllers = NULL;
static int controller_count = 0;
static controller_state_t* controller_get(uint8_t id);


void input_init(void) {
    for (int i = 0; i < KEYCODE_NUM_CODES; i++) {
        keyboard_codes[i] = 0;
    }

    mouse_state.x = 0;
    mouse_state.y = 0;
    mouse_state.motion_x = 0;
    mouse_state.motion_y = 0;
    mouse_state.buttons = 0;
    mouse_state.wheel_x = 0;
    mouse_state.wheel_y = 0;
}

void input_destroy(void) {

}

void input_update(void) {
    mouse_state.motion_x = 0;
    mouse_state.motion_y = 0;
    mouse_state.wheel_x = 0;
    mouse_state.wheel_y = 0;
}

void input_handle_event(event_t* event) {
    if (event->type == EVENT_KEYDOWN) {
        if (event->key.code < KEYCODE_NUM_CODES) {
            keyboard_codes[event->key.code] = 1;
        }
        if (event->key.symbol < KEYSYMBOL_NUM_SYMBOLS) {
            keyboard_symbols[event->key.symbol] = 1;
        }
    }
    else if (event->type == EVENT_KEYUP) {
        if (event->key.code < KEYCODE_NUM_CODES) {
            keyboard_codes[event->key.code] = 0;
        }
        if (event->key.symbol < KEYSYMBOL_NUM_SYMBOLS) {
            keyboard_symbols[event->key.symbol] = 0;
        }
    }
    else if (event->type == EVENT_MOUSEMOTION) {
        mouse_state.x = event->motion.x;
        mouse_state.y = event->motion.y;
        mouse_state.motion_x = event->motion.motion_x;
        mouse_state.motion_y = event->motion.motion_y;
    }
    else if (event->type == EVENT_MOUSEDOWN) {
        mouse_state.buttons |= (1 << event->button.button);
    }
    else if (event->type == EVENT_MOUSEUP) {
        mouse_state.buttons &= (~(1 <<  event->button.button));
    }
    else if (event->type == EVENT_MOUSEWHEEL) {
        mouse_state.wheel_x = event->wheel.wheel_x;
        mouse_state.wheel_y = event->wheel.wheel_y;
    }
    else if (event->type == EVENT_CONTROLLERBUTTONDOWN) {
        controller_state_t* controller = controller_get(event->controller_button.which);
        if (controller) {
            controller->buttons[event->controller_button.button] = 1;
        }
    }
    else if (event->type == EVENT_CONTROLLERBUTTONUP) {
        controller_state_t* controller = controller_get(event->controller_button.which);
        if (controller) {
            controller->buttons[event->controller_button.button] = 0;
        }
    }
    else if (event->type == EVENT_CONTROLLERAXISMOTION) {
        controller_state_t* controller = controller_get(event->controller_axis.which);
        if (controller) {
            controller->axis[event->controller_axis.axis] = event->controller_axis.value;
        }
    }
}

bool input_keyboard_is_key_code_down(key_code_t code) {
    if (code < 0) return false;
    if (code > KEYCODE_NUM_CODES) return false;

    return keyboard_codes[code] == 1;
}

bool input_keyboard_is_key_symbol_down(key_symbol_t symbol) {
    if (symbol < 0) return false;
    if (symbol > KEYSYMBOL_NUM_SYMBOLS) return false;

    return keyboard_symbols[symbol] == 1;
}

bool input_mouse_is_button_pressed(int button) {
    return mouse_state.buttons & (1 << button);
}

void input_mouse_position(int* x, int* y) {
    *x = mouse_state.x;
    *y = mouse_state.y;
}

void input_mouse_motion(int* x, int* y) {
    *x = mouse_state.motion_x;
    *y = mouse_state.motion_y;
}

void input_mouse_wheel(int* x, int* y) {
    *x = mouse_state.wheel_x;
    *y = mouse_state.wheel_y;
}

static controller_state_t* controller_get(uint8_t id) {
    if (!controllers) return NULL;

    controller_state_t* controller = NULL;
    for (controller = controllers; controller; controller = controller->next) {
        if (controller->id == id) return controller;
    }

    return NULL;
}

bool input_controller_is_button_pressed(uint8_t id, int button) {
    controller_state_t* controller = controller_get(id);
    if (!controller) return false;

    return controller->buttons[button];
}

void input_controller_axis(uint8_t id, int axis, float* value) {
    controller_state_t* controller = controller_get(id);
    if (!controller) return;

    *value = controller->axis[axis];
}

void input_controller_connect(uint8_t id) {
    controller_state_t* controller = controller_get(id);

    // If controller exists we are done
    if (controller) return;

    // Create a new controller state
    controller = (controller_state_t*)malloc(sizeof(controller_state_t));
    controller->id = id;

    for (int i = 0; i < CONTROLLER_BUTTON_COUNT; i++) {
        controller->buttons[i] = 0;
    }

    for (int i = 0; i < CONTROLLER_AXIS_COUNT; i++) {
        controller->axis[i] = 0;
    }

    controller->next = NULL;

    // Check if this is the first controller
    if (!controllers) {
        controllers = controller;
    }
    else {
        controller_state_t* last = controllers;
        while(last->next) {
            last = last->next;
        }

        last->next = controller;
    }

    controller_count++;
}

void input_controller_disconnect(uint8_t id) {
    controller_state_t* controller = controller_get(id);

    if (!controller) return;

    if (controller == controllers) {
        controllers = controller->next;
    }
    else {
        controller_state_t* prev = controllers;
        while(prev->next) {
            if (prev->next == controller) break;
            prev = prev->next;
        }

        prev->next = controller->next;
    }

    free(controller);
    controller = NULL;

    controller_count--;
}

bool input_controller_connected(uint8_t id) {
    controller_state_t* controller = controller_get(id);
    return (bool)controller;
}

int input_controller_count(void) {
    return controller_count;
}

void input_controller_ids(uint8_t* ids) {
    controller_state_t* controller = controllers;
    for (int i = 0; i < controller_count; i++) {
        ids[i] = controller->id;
        controller = controller->next;
    }
}
