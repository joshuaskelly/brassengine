--- @meta

--- Module for getting keyboard input.
local keyboard = {}

--- Check if given key is down.
--- @param code_or_symbol integer|string  Key code or symbol to check.
--- @return boolean True if key is down.
function keyboard.key(code_or_symbol) end

--- Key scancode nums
keyboard.scancodes = {
    A = 4,
    B = 5,
    C = 6,
    D = 7,
    E = 8,
    F = 9,
    G = 10,
    H = 11,
    I = 12,
    J = 13,
    K = 14,
    L = 15,
    M = 16,
    N = 17,
    O = 18,
    P = 19,
    Q = 20,
    R = 21,
    S = 22,
    T = 23,
    U = 24,
    V = 25,
    W = 26,
    X = 27,
    Y = 28,
    Z = 29,
    ONE = 30,
    TWO = 31,
    THREE = 32,
    FOUR = 33,
    FIVE = 34,
    SIX = 35,
    SEVEN = 36,
    EIGHT = 37,
    NINE = 38,
    ZERO = 39,
    RETURN = 40,
    ESCAPE = 41,
    BACKSPACE = 42,
    TAB = 43,
    SPACE = 44,
    MINUS = 45,
    EQUALS = 46,
    LEFT_BRACKET = 47,
    RIGHT_BRACKET = 48,
    BACKSLASH = 49,
    NON_US_HASH = 50,
    SEMICOLON = 51,
    APOSTROPHE = 52,
    GRAVE = 53,
    COMMA = 54,
    PERIOD = 55,
    SLASH = 56,
    CAPS_LOCK = 57,
    F1 = 58,
    F2 = 59,
    F3 = 60,
    F4 = 61,
    F5 = 62,
    F6 = 63,
    F7 = 64,
    F8 = 65,
    F9 = 66,
    F10 = 67,
    F11 = 68,
    F12 = 69,
    PRINT_SCREEN = 70,
    SCROLL_LOCK = 71,
    PAUSE = 72,
    INSERT = 73,
    HOME = 74,
    PAGE_UP = 75,
    DELETE = 76,
    END = 77,
    PAGE_DOWN = 78,
    RIGHT = 79,
    LEFT = 80,
    DOWN = 81,
    UP = 82,
    NUM_LOCK_CLEAR = 83,
    KEYPAD_DIVIDE = 84,
    KEYPAD_MULTIPLY = 85,
    KEYPAD_MINUS = 86,
    KEYPAD_PLUS = 87,
    KEYPAD_ENTER = 88,
    KEYPAD_1 = 89,
    KEYPAD_2 = 90,
    KEYPAD_3 = 91,
    KEYPAD_4 = 92,
    KEYPAD_5 = 93,
    KEYPAD_6 = 94,
    KEYPAD_7 = 95,
    KEYPAD_8 = 96,
    KEYPAD_9 = 97,
    KEYPAD_0 = 98,
    KEYPAD_PERIOD = 99,
    NON_US_BACKSLASH = 100,
    APPLICATION = 101,
    POWER = 102,
    KP_EQUALS = 103,
    F13 = 104,
    F14 = 105,
    F15 = 106,
    F16 = 107,
    F17 = 108,
    F18 = 109,
    F19 = 110,
    F20 = 111,
    F21 = 112,
    F22 = 113,
    F23 = 114,
    F24 = 115,
    EXECUTE = 116,
    HELP = 117,
    MENU = 118,
    SELECT = 119,
    STOP = 120,
    AGAIN = 121,
    UNDO = 122,
    CUT = 123,
    COPY = 124,
    PASTE = 125,
    FIND = 126,
    MUTE = 127,
    VOLUME_UP = 128,
    VOLUME_DOWN = 129,
    KP_COMMA = 133,
    KP_EQUALSAS400 = 134,
    INTERNATIONAL1 = 135,
    INTERNATIONAL2 = 136,
    INTERNATIONAL3 = 137,
    INTERNATIONAL4 = 138,
    INTERNATIONAL5 = 139,
    INTERNATIONAL6 = 140,
    INTERNATIONAL7 = 141,
    INTERNATIONAL8 = 142,
    INTERNATIONAL9 = 143,
    LANG1 = 144,
    LANG2 = 145,
    LANG3 = 146,
    LANG4 = 147,
    LANG5 = 148,
    LANG6 = 149,
    LANG7 = 150,
    LANG8 = 151,
    LANG9 = 152,
    ALT_ERASE = 153,
    SYS_REQ = 154,
    CANCEL = 155,
    CLEAR = 156,
    PRIOR = 157,
    RETURN2 = 158,
    SEPARATOR = 159,
    OUT = 160,
    OPER = 161,
    CLEAR_AGAIN = 162,
    CRSEL = 163,
    EXSEL = 164,
    KP_00 = 176,
    KP_000 = 177,
    THOUSANDS_SEPARATOR = 178,
    DECIMAL_SEPARATOR = 179,
    CURRENCY_UNIT = 180,
    CURRENCY_SUBUNIT = 181,
    KP_LEFT_PAREN = 182,
    KP_RIGHT_PAREN = 183,
    KP_LEFT_BRACE = 184,
    KP_RIGHT_BRACE = 185,
    KP_TAB = 186,
    KP_BACKSPACE = 187,
    KP_A = 188,
    KP_B = 189,
    KP_C = 190,
    KP_D = 191,
    KP_E = 192,
    KP_F = 193,
    KP_XOR = 194,
    KP_POWER = 195,
    KP_PERCENT = 196,
    KP_LESS = 197,
    KP_GREATER = 198,
    KP_AMPERSAND = 199,
    KP_DBL_AMPERSAND = 200,
    KP_VERTICAL_BAR = 201,
    KP_DBL_VERTICAL_BAR = 202,
    KP_COLON = 203,
    KP_HASH = 204,
    KP_SPACE = 205,
    KP_AT = 206,
    KP_EXCLAM = 207,
    KP_MEM_STORE = 208,
    KP_MEM_RECALL = 209,
    KP_MEM_CLEAR = 210,
    KP_MEM_ADD = 211,
    KP_MEM_SUBTRACT = 212,
    KP_MEM_MULTIPLY = 213,
    KP_MEM_DIVIDE = 214,
    KP_PLUS_MINUS = 215,
    KP_CLEAR = 216,
    KP_CLEAR_ENTRY = 217,
    KP_BINARY = 218,
    KP_OCTAL = 219,
    KP_DECIMAL = 220,
    KP_HEXADECIMAL = 221,
    LEFT_CTRL = 224,
    LEFT_SHIFT = 225,
    LEFT_ALT = 226,
    LEFT_GUI = 227,
    RIGHT_CTRL = 228,
    RIGHT_SHIFT = 229,
    RIGHT_ALT = 230,
    RIGHT_GUI = 231,
    MODE = 257,
    AUDIO_NEXT = 258,
    AUDIO_PREV = 259,
    AUDIO_STOP = 260,
    AUDIO_PLAY = 261,
    AUDIO_MUTE = 262,
    MEDIA_SELECT = 263,
    WWW = 264,
    MAIL = 265,
    CALCULATOR = 266,
    COMPUTER = 267,
    AC_SEARCH = 268,
    AC_HOME = 269,
    AC_BACK = 270,
    AC_FORWARD = 271,
    AC_STOP = 272,
    AC_REFRESH = 273,
    AC_BOOKMARKS = 274,
    BRIGHTNESS_DOWN = 275,
    BRIGHTNESS_UP = 276,
    DISPLAY_SWITCH = 277,
    KBD_ILLUM_TOGGLE = 278,
    KBD_ILLUM_DOWN = 279,
    KBD_ILLUM_UP = 280,
    EJECT = 281,
    SLEEP = 282,
    APP1 = 283,
    APP2 = 284,
    AUDIO_REWIND = 285,
    AUDIO_FAST_FORWARD = 286,
}

return keyboard