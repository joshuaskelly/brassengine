#include <stdbool.h>
#include <stdint.h>

#include "event.h"
#include "input.h"

static uint8_t keyboard_codes[KEYCODE_NUM_CODES];
static uint8_t keyboard_symbols[KEYSYMBOL_NUM_SYMBOLS];

static struct {
    int x;
    int y;
    int motion_x;
    int motion_y;
    uint32_t buttons;
} mouse_state;

void input_init(void) {
    for (int i = 0; i < KEYCODE_NUM_CODES; i++) {
        keyboard_codes[i] = 0;
    }

    mouse_state.x = 0;
    mouse_state.y = 0;
    mouse_state.motion_x = 0;
    mouse_state.motion_y = 0;
    mouse_state.buttons = 0;
}

void input_destroy(void) {

}

void input_update(void) {
    mouse_state.motion_x = 0;
    mouse_state.motion_y = 0;
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
