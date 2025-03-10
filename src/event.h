/**
 * @file event.h
 * Event handling module.
 */

#ifndef EVENT_H
#define EVENT_H

#include <stdbool.h>
#include <stdint.h>

#define MAXEVENTS 64

typedef enum {
    EVENT_UNSET = 0,
    EVENT_QUIT,
    EVENT_RELOAD,
    EVENT_KEYDOWN,
    EVENT_KEYUP,
    EVENT_MOUSEMOTION,
    EVENT_MOUSEDOWN,
    EVENT_MOUSEUP,
    EVENT_MOUSEWHEEL,
    EVENT_CONTROLLERBUTTONDOWN,
    EVENT_CONTROLLERBUTTONUP,
    EVENT_CONTROLLERAXISMOTION
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
    KEYCODE_NONUSBACKSLASH = 100,
    KEYCODE_APPLICATION = 101,
    KEYCODE_POWER = 102,
    KEYCODE_KP_EQUALS = 103,
    KEYCODE_F13 = 104,
    KEYCODE_F14 = 105,
    KEYCODE_F15 = 106,
    KEYCODE_F16 = 107,
    KEYCODE_F17 = 108,
    KEYCODE_F18 = 109,
    KEYCODE_F19 = 110,
    KEYCODE_F20 = 111,
    KEYCODE_F21 = 112,
    KEYCODE_F22 = 113,
    KEYCODE_F23 = 114,
    KEYCODE_F24 = 115,
    KEYCODE_EXECUTE = 116,
    KEYCODE_HELP = 117,
    KEYCODE_MENU = 118,
    KEYCODE_SELECT = 119,
    KEYCODE_STOP = 120,
    KEYCODE_AGAIN = 121,
    KEYCODE_UNDO = 122,
    KEYCODE_CUT = 123,
    KEYCODE_COPY = 124,
    KEYCODE_PASTE = 125,
    KEYCODE_FIND = 126,
    KEYCODE_MUTE = 127,
    KEYCODE_VOLUMEUP = 128,
    KEYCODE_VOLUMEDOWN = 129,
    KEYCODE_KP_COMMA = 133,
    KEYCODE_KP_EQUALSAS400 = 134,
    KEYCODE_INTERNATIONAL1 = 135,
    KEYCODE_INTERNATIONAL2 = 136,
    KEYCODE_INTERNATIONAL3 = 137,
    KEYCODE_INTERNATIONAL4 = 138,
    KEYCODE_INTERNATIONAL5 = 139,
    KEYCODE_INTERNATIONAL6 = 140,
    KEYCODE_INTERNATIONAL7 = 141,
    KEYCODE_INTERNATIONAL8 = 142,
    KEYCODE_INTERNATIONAL9 = 143,
    KEYCODE_LANG1 = 144,
    KEYCODE_LANG2 = 145,
    KEYCODE_LANG3 = 146,
    KEYCODE_LANG4 = 147,
    KEYCODE_LANG5 = 148,
    KEYCODE_LANG6 = 149,
    KEYCODE_LANG7 = 150,
    KEYCODE_LANG8 = 151,
    KEYCODE_LANG9 = 152,
    KEYCODE_ALTERASE = 153,
    KEYCODE_SYSREQ = 154,
    KEYCODE_CANCEL = 155,
    KEYCODE_CLEAR = 156,
    KEYCODE_PRIOR = 157,
    KEYCODE_RETURN2 = 158,
    KEYCODE_SEPARATOR = 159,
    KEYCODE_OUT = 160,
    KEYCODE_OPER = 161,
    KEYCODE_CLEARAGAIN = 162,
    KEYCODE_CRSEL = 163,
    KEYCODE_EXSEL = 164,
    KEYCODE_KP_00 = 176,
    KEYCODE_KP_000 = 177,
    KEYCODE_THOUSANDSSEPARATOR = 178,
    KEYCODE_DECIMALSEPARATOR = 179,
    KEYCODE_CURRENCYUNIT = 180,
    KEYCODE_CURRENCYSUBUNIT = 181,
    KEYCODE_KP_LEFTPAREN = 182,
    KEYCODE_KP_RIGHTPAREN = 183,
    KEYCODE_KP_LEFTBRACE = 184,
    KEYCODE_KP_RIGHTBRACE = 185,
    KEYCODE_KP_TAB = 186,
    KEYCODE_KP_BACKSPACE = 187,
    KEYCODE_KP_A = 188,
    KEYCODE_KP_B = 189,
    KEYCODE_KP_C = 190,
    KEYCODE_KP_D = 191,
    KEYCODE_KP_E = 192,
    KEYCODE_KP_F = 193,
    KEYCODE_KP_XOR = 194,
    KEYCODE_KP_POWER = 195,
    KEYCODE_KP_PERCENT = 196,
    KEYCODE_KP_LESS = 197,
    KEYCODE_KP_GREATER = 198,
    KEYCODE_KP_AMPERSAND = 199,
    KEYCODE_KP_DBLAMPERSAND = 200,
    KEYCODE_KP_VERTICALBAR = 201,
    KEYCODE_KP_DBLVERTICALBAR = 202,
    KEYCODE_KP_COLON = 203,
    KEYCODE_KP_HASH = 204,
    KEYCODE_KP_SPACE = 205,
    KEYCODE_KP_AT = 206,
    KEYCODE_KP_EXCLAM = 207,
    KEYCODE_KP_MEMSTORE = 208,
    KEYCODE_KP_MEMRECALL = 209,
    KEYCODE_KP_MEMCLEAR = 210,
    KEYCODE_KP_MEMADD = 211,
    KEYCODE_KP_MEMSUBTRACT = 212,
    KEYCODE_KP_MEMMULTIPLY = 213,
    KEYCODE_KP_MEMDIVIDE = 214,
    KEYCODE_KP_PLUSMINUS = 215,
    KEYCODE_KP_CLEAR = 216,
    KEYCODE_KP_CLEARENTRY = 217,
    KEYCODE_KP_BINARY = 218,
    KEYCODE_KP_OCTAL = 219,
    KEYCODE_KP_DECIMAL = 220,
    KEYCODE_KP_HEXADECIMAL = 221,
    KEYCODE_LCTRL = 224,
    KEYCODE_LSHIFT = 225,
    KEYCODE_LALT = 226,
    KEYCODE_LGUI = 227,
    KEYCODE_RCTRL = 228,
    KEYCODE_RSHIFT = 229,
    KEYCODE_RALT = 230,
    KEYCODE_RGUI = 231,
    KEYCODE_MODE = 257,
    KEYCODE_AUDIONEXT = 258,
    KEYCODE_AUDIOPREV = 259,
    KEYCODE_AUDIOSTOP = 260,
    KEYCODE_AUDIOPLAY = 261,
    KEYCODE_AUDIOMUTE = 262,
    KEYCODE_MEDIASELECT = 263,
    KEYCODE_WWW = 264,
    KEYCODE_MAIL = 265,
    KEYCODE_CALCULATOR = 266,
    KEYCODE_COMPUTER = 267,
    KEYCODE_AC_SEARCH = 268,
    KEYCODE_AC_HOME = 269,
    KEYCODE_AC_BACK = 270,
    KEYCODE_AC_FORWARD = 271,
    KEYCODE_AC_STOP = 272,
    KEYCODE_AC_REFRESH = 273,
    KEYCODE_AC_BOOKMARKS = 274,
    KEYCODE_BRIGHTNESSDOWN = 275,
    KEYCODE_BRIGHTNESSUP = 276,
    KEYCODE_DISPLAYSWITCH = 277,
    KEYCODE_KBDILLUMTOGGLE = 278,
    KEYCODE_KBDILLUMDOWN = 279,
    KEYCODE_KBDILLUMUP = 280,
    KEYCODE_EJECT = 281,
    KEYCODE_SLEEP = 282,
    KEYCODE_APP1 = 283,
    KEYCODE_APP2 = 284,
    KEYCODE_AUDIOREWIND = 285,
    KEYCODE_AUDIOFASTFORWARD = 286,
    KEYCODE_NUM_CODES = 512
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
    KEYSYMBOL_NUM_SYMBOLS = 128,
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
    int motion_x;
    int motion_y;
} mouse_motion_event_t;

typedef struct {
    event_type_t type;
    uint8_t button;
} mouse_button_event_t;

typedef struct {
    event_type_t type;
    int wheel_x;
    int wheel_y;
} mouse_wheel_event_t;

typedef enum {
    CONTROLLER_BUTTON_UNKNOWN = -1,
    CONTROLLER_BUTTON_A = 0,
    CONTROLLER_BUTTON_B = 1,
    CONTROLLER_BUTTON_X = 2,
    CONTROLLER_BUTTON_Y = 3,
    CONTROLLER_BUTTON_BACK = 4,
    CONTROLLER_BUTTON_GUIDE = 5,
    CONTROLLER_BUTTON_START = 6,
    CONTROLLER_BUTTON_LEFT_STICK = 7,
    CONTROLLER_BUTTON_RIGHT_STICK = 8,
    CONTROLLER_BUTTON_LEFT_SHOULDER = 9,
    CONTROLLER_BUTTON_RIGHT_SHOULDER = 10,
    CONTROLLER_BUTTON_DPAD_UP = 11,
    CONTROLLER_BUTTON_DPAD_DOWN = 12,
    CONTROLLER_BUTTON_DPAD_LEFT = 13,
    CONTROLLER_BUTTON_DPAD_RIGHT = 14,
    CONTROLLER_BUTTON_COUNT = 15
} controller_button_t;

typedef struct {
    event_type_t type;
    uint8_t which;
    controller_button_t button;
} controller_button_event_t;

typedef enum {
    CONTROLLER_AXIS_UNKNOWN = -1,
    CONTROLLER_AXIS_LEFT_X = 0,
    CONTROLLER_AXIS_LEFT_Y = 1,
    CONTROLLER_AXIS_RIGHT_X = 2,
    CONTROLLER_AXIS_RIGHT_Y = 3,
    CONTROLLER_AXIS_TRIGGER_LEFT = 4,
    CONTROLLER_AXIS_TRIGGER_RIGHT = 5,
    CONTROLLER_AXIS_COUNT = 6
} controller_axis_t;

typedef struct {
    event_type_t type;
    controller_axis_t axis;
    uint8_t which;
    float value;
} controller_axis_event_t;

typedef union {
    event_type_t type;
    common_event_t common;
    key_event_t key;
    mouse_motion_event_t motion;
    mouse_button_event_t button;
    mouse_wheel_event_t wheel;
    controller_button_event_t controller_button;
    controller_axis_event_t controller_axis;
} event_t;

/**
 * Push an event into the event queue.
 *
 * @param event An event_t structure to be copied into the queue.
 */
void event_post(event_t*  event);

/**
 * Poll for pending events.
 *
 * @param event An event_t structure to be set from the next event
 * in the queue.
 * @return true if there is a pending event, false if none.
 */
bool event_poll(event_t* event);

#endif
