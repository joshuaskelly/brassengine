/**
 * @file event.h
 * @brief Event handling module.
 */

#ifndef EVENT_H
#define EVENT_H

#include <stdbool.h>

#define MAXEVENTS 64

typedef enum {
    EVENT_UNSET = 0,
    EVENT_QUIT = 1,
    EVENT_KEYDOWN = 2,
    EVENT_KEYUP = 3,
    EVENT_MOUSEMOTION,
} event_type_t;

typedef struct {
    event_type_t type;
} common_event_t;

typedef enum {
    KEYCODE_UNKNOWN = 0,
    KEYCODE_A = 4,
    KEYCODE_B = 5,
    KEYCODE_C = 6,
    KEYCODE_D = 7,
    KEYCODE_E = 8,
    KEYCODE_F = 9,
    KEYCODE_G = 10,
    KEYCODE_H = 11,
    KEYCODE_I = 12,
    KEYCODE_J = 13,
    KEYCODE_K = 14,
    KEYCODE_L = 15,
    KEYCODE_M = 16,
    KEYCODE_N = 17,
    KEYCODE_O = 18,
    KEYCODE_P = 19,
    KEYCODE_Q = 20,
    KEYCODE_R = 21,
    KEYCODE_S = 22,
    KEYCODE_T = 23,
    KEYCODE_U = 24,
    KEYCODE_V = 25,
    KEYCODE_W = 26,
    KEYCODE_X = 27,
    KEYCODE_Y = 28,
    KEYCODE_Z = 29,
    KEYCODE_1 = 30,
    KEYCODE_2 = 31,
    KEYCODE_3 = 32,
    KEYCODE_4 = 33,
    KEYCODE_5 = 34,
    KEYCODE_6 = 35,
    KEYCODE_7 = 36,
    KEYCODE_8 = 37,
    KEYCODE_9 = 38,
    KEYCODE_0 = 39,
    KEYCODE_RETURN = 40,
    KEYCODE_ESCAPE = 41,
    KEYCODE_BACKSPACE = 42,
    KEYCODE_TAB = 43,
    KEYCODE_SPACE = 44,
    KEYCODE_MINUS = 45,
    KEYCODE_EQUALS = 46,
    KEYCODE_LEFTBRACKET = 47,
    KEYCODE_RIGHTBRACKET = 48,
    KEYCODE_BACKSLASH = 49,
    KEYCODE_NONUSHASH = 50,
    KEYCODE_SEMICOLON = 51,
    KEYCODE_APOSTROPHE = 52,
    KEYCODE_GRAVE = 53,
    KEYCODE_COMMA = 54,
    KEYCODE_PERIOD = 55,
    KEYCODE_SLASH = 56,
    KEYCODE_CAPSLOCK = 57,
    KEYCODE_F1 = 58,
    KEYCODE_F2 = 59,
    KEYCODE_F3 = 60,
    KEYCODE_F4 = 61,
    KEYCODE_F5 = 62,
    KEYCODE_F6 = 63,
    KEYCODE_F7 = 64,
    KEYCODE_F8 = 65,
    KEYCODE_F9 = 66,
    KEYCODE_F10 = 67,
    KEYCODE_F11 = 68,
    KEYCODE_F12 = 69,
    KEYCODE_PRINTSCREEN = 70,
    KEYCODE_SCROLLLOCK = 71,
    KEYCODE_PAUSE = 72,
    KEYCODE_INSERT = 73,
    KEYCODE_HOME = 74,
    KEYCODE_PAGEUP = 75,
    KEYCODE_DELETE = 76,
    KEYCODE_END = 77,
    KEYCODE_PAGEDOWN = 78,
    KEYCODE_RIGHT = 79,
    KEYCODE_LEFT = 80,
    KEYCODE_DOWN = 81,
    KEYCODE_UP = 82,
    KEYCODE_NUMLOCKCLEAR = 83,
    KEYCODE_KEYPAD_DIVIDE = 84,
    KEYCODE_KEYPAD_MULTIPLY = 85,
    KEYCODE_KEYPAD_MINUS = 86,
    KEYCODE_KEYPAD_PLUS = 87,
    KEYCODE_KEYPAD_ENTER = 88,
    KEYCODE_KEYPAD_1 = 89,
    KEYCODE_KEYPAD_2 = 90,
    KEYCODE_KEYPAD_3 = 91,
    KEYCODE_KEYPAD_4 = 92,
    KEYCODE_KEYPAD_5 = 93,
    KEYCODE_KEYPAD_6 = 94,
    KEYCODE_KEYPAD_7 = 95,
    KEYCODE_KEYPAD_8 = 96,
    KEYCODE_KEYPAD_9 = 97,
    KEYCODE_KEYPAD_0 = 98,
    KEYCODE_KEYPAD_PERIOD = 99,
} key_code_t;

typedef enum {
    KEYSYMBOL_UNKNOWN = 0,
    KEYSYMBOL_RETURN = '\r',
    KEYSYMBOL_ESCAPE = '\x1B',
    KEYSYMBOL_BACKSPACE = '\b',
    KEYSYMBOL_TAB = '\t',
    KEYSYMBOL_SPACE = ' ',
    KEYSYMBOL_EXCLAIM = '!',
    KEYSYMBOL_QUOTEDBL = '"',
    KEYSYMBOL_HASH = '#',
    KEYSYMBOL_PERCENT = '%',
    KEYSYMBOL_DOLLAR = '$',
    KEYSYMBOL_AMPERSAND = '&',
    KEYSYMBOL_QUOTE = '\'',
    KEYSYMBOL_LEFTPAREN = '(',
    KEYSYMBOL_RIGHTPAREN = ')',
    KEYSYMBOL_ASTERISK = '*',
    KEYSYMBOL_PLUS = '+',
    KEYSYMBOL_COMMA = ',',
    KEYSYMBOL_MINUS = '-',
    KEYSYMBOL_PERIOD = '.',
    KEYSYMBOL_SLASH = '/',
    KEYSYMBOL_0 = '0',
    KEYSYMBOL_1 = '1',
    KEYSYMBOL_2 = '2',
    KEYSYMBOL_3 = '3',
    KEYSYMBOL_4 = '4',
    KEYSYMBOL_5 = '5',
    KEYSYMBOL_6 = '6',
    KEYSYMBOL_7 = '7',
    KEYSYMBOL_8 = '8',
    KEYSYMBOL_9 = '9',
    KEYSYMBOL_COLON = ':',
    KEYSYMBOL_SEMICOLON = ';',
    KEYSYMBOL_LESS = '<',
    KEYSYMBOL_EQUALS = '=',
    KEYSYMBOL_GREATER = '>',
    KEYSYMBOL_QUESTION = '?',
    KEYSYMBOL_AT = '@',
    KEYSYMBOL_LEFTBRACKET = '[',
    KEYSYMBOL_BACKSLASH = '\\',
    KEYSYMBOL_RIGHTBRACKET = ']',
    KEYSYMBOL_CARET = '^',
    KEYSYMBOL_UNDERSCORE = '_',
    KEYSYMBOL_BACKQUOTE = '`',
    KEYSYMBOL_a = 'a',
    KEYSYMBOL_b = 'b',
    KEYSYMBOL_c = 'c',
    KEYSYMBOL_d = 'd',
    KEYSYMBOL_e = 'e',
    KEYSYMBOL_f = 'f',
    KEYSYMBOL_g = 'g',
    KEYSYMBOL_h = 'h',
    KEYSYMBOL_i = 'i',
    KEYSYMBOL_j = 'j',
    KEYSYMBOL_k = 'k',
    KEYSYMBOL_l = 'l',
    KEYSYMBOL_m = 'm',
    KEYSYMBOL_n = 'n',
    KEYSYMBOL_o = 'o',
    KEYSYMBOL_p = 'p',
    KEYSYMBOL_q = 'q',
    KEYSYMBOL_r = 'r',
    KEYSYMBOL_s = 's',
    KEYSYMBOL_t = 't',
    KEYSYMBOL_u = 'u',
    KEYSYMBOL_v = 'v',
    KEYSYMBOL_w = 'w',
    KEYSYMBOL_x = 'x',
    KEYSYMBOL_y = 'y',
    KEYSYMBOL_z = 'z',
} key_symbol_t;

typedef struct {
    event_type_t type;
    key_code_t code;
    key_symbol_t symbol;
} key_event_t;

typedef struct {
    event_type_t type;
    int x;
    int y;
    int rel_x;
    int rel_y;
} mousemotion_event_t;

typedef union {
    event_type_t type;
    common_event_t common;
    key_event_t key;
    mousemotion_event_t motion;
} event_t;

/**
 * @brief Push an event into the event queue.
 *
 * @param event An event_t structure to be copied into the queue.
 */
void event_post(event_t*  event);

/**
 * @brief Poll for pending events.
 *
 * @param event An event_t structure to be set from the next event
 * in the queue.
 * @return true if there is a pending event, false if none.
 */
bool event_poll(event_t* event);

#endif