/**
 * Module for getting keyboard input.
 * @module keyboard
 */
#include <ctype.h>
#include <stdbool.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "keyboard.h"

#include "../input.h"

/**
 * Check if given key is down.
 * @function key
 * @tparam integer|string code_or_symbol Key code or symbol to check.
 * @treturn boolean True if key is down.
 */
static int modules_keyboard_key_get(lua_State* L) {
    // Key code
    if (lua_isinteger(L, 1)) {
        int key_code = luaL_checkinteger(L, 1);
        lua_pop(L, -1);
        bool is_down = input_keyboard_is_key_code_down(key_code);
        lua_pushboolean(L, is_down);
    }
    // Key symbol
    else {
        const char* string = luaL_checkstring(L, 1);
        char key_symbol = tolower(string[0]);
        lua_pop(L, -1);
        bool is_down = input_keyboard_is_key_symbol_down(key_symbol);
        lua_pushboolean(L, is_down);
    }

    return 1;
}

struct luaL_Field {
  const char *name;
  int value;
};

/**
 * Enums
 * @section Enums
 */

/**
 * Scancode enums
 * @table scancodes
 *
 * @tfield integer A
 * @tfield integer B
 * @tfield integer C
 * @tfield integer D
 * @tfield integer E
 * @tfield integer F
 * @tfield integer G
 * @tfield integer H
 * @tfield integer I
 * @tfield integer J
 * @tfield integer K
 * @tfield integer L
 * @tfield integer M
 * @tfield integer N
 * @tfield integer O
 * @tfield integer P
 * @tfield integer Q
 * @tfield integer R
 * @tfield integer S
 * @tfield integer T
 * @tfield integer U
 * @tfield integer V
 * @tfield integer W
 * @tfield integer X
 * @tfield integer Y
 * @tfield integer Z
 * @tfield integer ONE
 * @tfield integer TWO
 * @tfield integer THREE
 * @tfield integer FOUR
 * @tfield integer FIVE
 * @tfield integer SIX
 * @tfield integer SEVEN
 * @tfield integer EIGHT
 * @tfield integer NINE
 * @tfield integer ZERO
 * @tfield integer RETURN
 * @tfield integer ESCAPE
 * @tfield integer BACKSPACE
 * @tfield integer TAB
 * @tfield integer SPACE
 * @tfield integer MINUS
 * @tfield integer EQUALS
 * @tfield integer LEFT_BRACKET
 * @tfield integer RIGHT_BRACKET
 * @tfield integer BACKSLASH
 * @tfield integer NON_US_HASH
 * @tfield integer SEMICOLON
 * @tfield integer APOSTROPHE
 * @tfield integer GRAVE
 * @tfield integer COMMA
 * @tfield integer PERIOD
 * @tfield integer SLASH
 * @tfield integer CAPS_LOCK
 * @tfield integer F1
 * @tfield integer F2
 * @tfield integer F3
 * @tfield integer F4
 * @tfield integer F5
 * @tfield integer F6
 * @tfield integer F7
 * @tfield integer F8
 * @tfield integer F9
 * @tfield integer F10
 * @tfield integer F11
 * @tfield integer F12
 * @tfield integer PRINT_SCREEN
 * @tfield integer SCROLL_LOCK
 * @tfield integer PAUSE
 * @tfield integer INSERT
 * @tfield integer HOME
 * @tfield integer PAGE_UP
 * @tfield integer DELETE
 * @tfield integer END
 * @tfield integer PAGE_DOWN
 * @tfield integer RIGHT
 * @tfield integer LEFT
 * @tfield integer DOWN
 * @tfield integer UP
 * @tfield integer NUM_LOCK_CLEAR
 * @tfield integer KEYPAD_DIVIDE
 * @tfield integer KEYPAD_MULTIPLY
 * @tfield integer KEYPAD_MINUS
 * @tfield integer KEYPAD_PLUS
 * @tfield integer KEYPAD_ENTER
 * @tfield integer KEYPAD_1
 * @tfield integer KEYPAD_2
 * @tfield integer KEYPAD_3
 * @tfield integer KEYPAD_4
 * @tfield integer KEYPAD_5
 * @tfield integer KEYPAD_6
 * @tfield integer KEYPAD_7
 * @tfield integer KEYPAD_8
 * @tfield integer KEYPAD_9
 * @tfield integer KEYPAD_0
 * @tfield integer KEYPAD_PERIOD
 * @tfield integer NON_US_BACKSLASH
 * @tfield integer APPLICATION
 * @tfield integer POWER
 * @tfield integer KP_EQUALS
 * @tfield integer F13
 * @tfield integer F14
 * @tfield integer F15
 * @tfield integer F16
 * @tfield integer F17
 * @tfield integer F18
 * @tfield integer F19
 * @tfield integer F20
 * @tfield integer F21
 * @tfield integer F22
 * @tfield integer F23
 * @tfield integer F24
 * @tfield integer EXECUTE
 * @tfield integer HELP
 * @tfield integer MENU
 * @tfield integer SELECT
 * @tfield integer STOP
 * @tfield integer AGAIN
 * @tfield integer UNDO
 * @tfield integer CUT
 * @tfield integer COPY
 * @tfield integer PASTE
 * @tfield integer FIND
 * @tfield integer MUTE
 * @tfield integer VOLUME_UP
 * @tfield integer VOLUME_DOWN
 * @tfield integer KP_COMMA
 * @tfield integer KP_EQUALSAS400
 * @tfield integer INTERNATIONAL1
 * @tfield integer INTERNATIONAL2
 * @tfield integer INTERNATIONAL3
 * @tfield integer INTERNATIONAL4
 * @tfield integer INTERNATIONAL5
 * @tfield integer INTERNATIONAL6
 * @tfield integer INTERNATIONAL7
 * @tfield integer INTERNATIONAL8
 * @tfield integer INTERNATIONAL9
 * @tfield integer LANG1
 * @tfield integer LANG2
 * @tfield integer LANG3
 * @tfield integer LANG4
 * @tfield integer LANG5
 * @tfield integer LANG6
 * @tfield integer LANG7
 * @tfield integer LANG8
 * @tfield integer LANG9
 * @tfield integer ALT_ERASE
 * @tfield integer SYS_REQ
 * @tfield integer CANCEL
 * @tfield integer CLEAR
 * @tfield integer PRIOR
 * @tfield integer RETURN2
 * @tfield integer SEPARATOR
 * @tfield integer OUT
 * @tfield integer OPER
 * @tfield integer CLEAR_AGAIN
 * @tfield integer CRSEL
 * @tfield integer EXSEL
 * @tfield integer KP_00
 * @tfield integer KP_000
 * @tfield integer THOUSANDS_SEPARATOR
 * @tfield integer DECIMAL_SEPARATOR
 * @tfield integer CURRENCY_UNIT
 * @tfield integer CURRENCY_SUBUNIT
 * @tfield integer KP_LEFT_PAREN
 * @tfield integer KP_RIGHT_PAREN
 * @tfield integer KP_LEFT_BRACE
 * @tfield integer KP_RIGHT_BRACE
 * @tfield integer KP_TAB
 * @tfield integer KP_BACKSPACE
 * @tfield integer KP_A
 * @tfield integer KP_B
 * @tfield integer KP_C
 * @tfield integer KP_D
 * @tfield integer KP_E
 * @tfield integer KP_F
 * @tfield integer KP_XOR
 * @tfield integer KP_POWER
 * @tfield integer KP_PERCENT
 * @tfield integer KP_LESS
 * @tfield integer KP_GREATER
 * @tfield integer KP_AMPERSAND
 * @tfield integer KP_DBL_AMPERSAND
 * @tfield integer KP_VERTICAL_BAR
 * @tfield integer KP_DBL_VERTICAL_BAR
 * @tfield integer KP_COLON
 * @tfield integer KP_HASH
 * @tfield integer KP_SPACE
 * @tfield integer KP_AT
 * @tfield integer KP_EXCLAM
 * @tfield integer KP_MEM_STORE
 * @tfield integer KP_MEM_RECALL
 * @tfield integer KP_MEM_CLEAR
 * @tfield integer KP_MEM_ADD
 * @tfield integer KP_MEM_SUBTRACT
 * @tfield integer KP_MEM_MULTIPLY
 * @tfield integer KP_MEM_DIVIDE
 * @tfield integer KP_PLUS_MINUS
 * @tfield integer KP_CLEAR
 * @tfield integer KP_CLEAR_ENTRY
 * @tfield integer KP_BINARY
 * @tfield integer KP_OCTAL
 * @tfield integer KP_DECIMAL
 * @tfield integer KP_HEXADECIMAL
 * @tfield integer LEFT_CTRL
 * @tfield integer LEFT_SHIFT
 * @tfield integer LEFT_ALT
 * @tfield integer LEFT_GUI
 * @tfield integer RIGHT_CTRL
 * @tfield integer RIGHT_SHIFT
 * @tfield integer RIGHT_ALT
 * @tfield integer RIGHT_GUI
 * @tfield integer MODE
 * @tfield integer AUDIO_NEXT
 * @tfield integer AUDIO_PREV
 * @tfield integer AUDIO_STOP
 * @tfield integer AUDIO_PLAY
 * @tfield integer AUDIO_MUTE
 * @tfield integer MEDIA_SELECT
 * @tfield integer WWW
 * @tfield integer MAIL
 * @tfield integer CALCULATOR
 * @tfield integer COMPUTER
 * @tfield integer AC_SEARCH
 * @tfield integer AC_HOME
 * @tfield integer AC_BACK
 * @tfield integer AC_FORWARD
 * @tfield integer AC_STOP
 * @tfield integer AC_REFRESH
 * @tfield integer AC_BOOKMARKS
 * @tfield integer BRIGHTNESS_DOWN
 * @tfield integer BRIGHTNESS_UP
 * @tfield integer DISPLAY_SWITCH
 * @tfield integer KBD_ILLUM_TOGGLE
 * @tfield integer KBD_ILLUM_DOWN
 * @tfield integer KBD_ILLUM_UP
 * @tfield integer EJECT
 * @tfield integer SLEEP
 * @tfield integer APP1
 * @tfield integer APP2
 * @tfield integer AUDIO_REWIND
 * @tfield integer AUDIO_FAST_FORWARD
 */

static const struct luaL_Field keyboard_scancodes[] = {
    {"A", KEYCODE_A},
    {"B", KEYCODE_B},
    {"C", KEYCODE_C},
    {"D", KEYCODE_D},
    {"E", KEYCODE_E},
    {"F", KEYCODE_F},
    {"G", KEYCODE_G},
    {"H", KEYCODE_H},
    {"I", KEYCODE_I},
    {"J", KEYCODE_J},
    {"K", KEYCODE_K},
    {"L", KEYCODE_L},
    {"M", KEYCODE_M},
    {"N", KEYCODE_N},
    {"O", KEYCODE_O},
    {"P", KEYCODE_P},
    {"Q", KEYCODE_Q},
    {"R", KEYCODE_R},
    {"S", KEYCODE_S},
    {"T", KEYCODE_T},
    {"U", KEYCODE_U},
    {"V", KEYCODE_V},
    {"W", KEYCODE_W},
    {"X", KEYCODE_X},
    {"Y", KEYCODE_Y},
    {"Z", KEYCODE_Z},
    {"ONE", KEYCODE_1},
    {"TWO", KEYCODE_2},
    {"THREE", KEYCODE_3},
    {"FOUR", KEYCODE_4},
    {"FIVE", KEYCODE_5},
    {"SIX", KEYCODE_6},
    {"SEVEN", KEYCODE_7},
    {"EIGHT", KEYCODE_8},
    {"NINE", KEYCODE_9},
    {"ZERO", KEYCODE_0},
    {"RETURN", KEYCODE_RETURN},
    {"ESCAPE", KEYCODE_ESCAPE},
    {"BACKSPACE", KEYCODE_BACKSPACE},
    {"TAB", KEYCODE_TAB},
    {"SPACE", KEYCODE_SPACE},
    {"MINUS", KEYCODE_MINUS},
    {"EQUALS", KEYCODE_EQUALS},
    {"LEFT_BRACKET", KEYCODE_LEFTBRACKET},
    {"RIGHT_BRACKET", KEYCODE_RIGHTBRACKET},
    {"BACKSLASH", KEYCODE_BACKSLASH},
    {"NON_US_HASH", KEYCODE_NONUSHASH},
    {"SEMICOLON", KEYCODE_SEMICOLON},
    {"APOSTROPHE", KEYCODE_APOSTROPHE},
    {"GRAVE", KEYCODE_GRAVE},
    {"COMMA", KEYCODE_COMMA},
    {"PERIOD", KEYCODE_PERIOD},
    {"SLASH", KEYCODE_SLASH},
    {"CAPS_LOCK", KEYCODE_CAPSLOCK},
    {"F1", KEYCODE_F1},
    {"F2", KEYCODE_F2},
    {"F3", KEYCODE_F3},
    {"F4", KEYCODE_F4},
    {"F5", KEYCODE_F5},
    {"F6", KEYCODE_F6},
    {"F7", KEYCODE_F7},
    {"F8", KEYCODE_F8},
    {"F9", KEYCODE_F9},
    {"F10", KEYCODE_F10},
    {"F11", KEYCODE_F11},
    {"F12", KEYCODE_F12},
    {"PRINT_SCREEN", KEYCODE_PRINTSCREEN},
    {"SCROLL_LOCK", KEYCODE_SCROLLLOCK},
    {"PAUSE", KEYCODE_PAUSE},
    {"INSERT", KEYCODE_INSERT},
    {"HOME", KEYCODE_HOME},
    {"PAGE_UP", KEYCODE_PAGEUP},
    {"DELETE", KEYCODE_DELETE},
    {"END", KEYCODE_END},
    {"PAGE_DOWN", KEYCODE_PAGEDOWN},
    {"RIGHT", KEYCODE_RIGHT},
    {"LEFT", KEYCODE_LEFT},
    {"DOWN", KEYCODE_DOWN},
    {"UP", KEYCODE_UP},
    {"NUM_LOCK_CLEAR", KEYCODE_NUMLOCKCLEAR},
    {"KEYPAD_DIVIDE", KEYCODE_KEYPAD_DIVIDE},
    {"KEYPAD_MULTIPLY", KEYCODE_KEYPAD_MULTIPLY},
    {"KEYPAD_MINUS", KEYCODE_KEYPAD_MINUS},
    {"KEYPAD_PLUS", KEYCODE_KEYPAD_PLUS},
    {"KEYPAD_ENTER", KEYCODE_KEYPAD_ENTER},
    {"KEYPAD_1", KEYCODE_KEYPAD_1},
    {"KEYPAD_2", KEYCODE_KEYPAD_2},
    {"KEYPAD_3", KEYCODE_KEYPAD_3},
    {"KEYPAD_4", KEYCODE_KEYPAD_4},
    {"KEYPAD_5", KEYCODE_KEYPAD_5},
    {"KEYPAD_6", KEYCODE_KEYPAD_6},
    {"KEYPAD_7", KEYCODE_KEYPAD_7},
    {"KEYPAD_8", KEYCODE_KEYPAD_8},
    {"KEYPAD_9", KEYCODE_KEYPAD_9},
    {"KEYPAD_0", KEYCODE_KEYPAD_0},
    {"KEYPAD_PERIOD", KEYCODE_KEYPAD_PERIOD},
    {"NON_US_BACKSLASH", KEYCODE_NONUSBACKSLASH},
    {"APPLICATION", KEYCODE_APPLICATION},
    {"POWER", KEYCODE_POWER},
    {"KP_EQUALS", KEYCODE_KP_EQUALS},
    {"F13", KEYCODE_F13},
    {"F14", KEYCODE_F14},
    {"F15", KEYCODE_F15},
    {"F16", KEYCODE_F16},
    {"F17", KEYCODE_F17},
    {"F18", KEYCODE_F18},
    {"F19", KEYCODE_F19},
    {"F20", KEYCODE_F20},
    {"F21", KEYCODE_F21},
    {"F22", KEYCODE_F22},
    {"F23", KEYCODE_F23},
    {"F24", KEYCODE_F24},
    {"EXECUTE", KEYCODE_EXECUTE},
    {"HELP", KEYCODE_HELP},
    {"MENU", KEYCODE_MENU},
    {"SELECT", KEYCODE_SELECT},
    {"STOP", KEYCODE_STOP},
    {"AGAIN", KEYCODE_AGAIN},
    {"UNDO", KEYCODE_UNDO},
    {"CUT", KEYCODE_CUT},
    {"COPY", KEYCODE_COPY},
    {"PASTE", KEYCODE_PASTE},
    {"FIND", KEYCODE_FIND},
    {"MUTE", KEYCODE_MUTE},
    {"VOLUME_UP", KEYCODE_VOLUMEUP},
    {"VOLUME_DOWN", KEYCODE_VOLUMEDOWN},
    {"KP_COMMA", KEYCODE_KP_COMMA},
    {"KP_EQUALSAS400", KEYCODE_KP_EQUALSAS400},
    {"INTERNATIONAL1", KEYCODE_INTERNATIONAL1},
    {"INTERNATIONAL2", KEYCODE_INTERNATIONAL2},
    {"INTERNATIONAL3", KEYCODE_INTERNATIONAL3},
    {"INTERNATIONAL4", KEYCODE_INTERNATIONAL4},
    {"INTERNATIONAL5", KEYCODE_INTERNATIONAL5},
    {"INTERNATIONAL6", KEYCODE_INTERNATIONAL6},
    {"INTERNATIONAL7", KEYCODE_INTERNATIONAL7},
    {"INTERNATIONAL8", KEYCODE_INTERNATIONAL8},
    {"INTERNATIONAL9", KEYCODE_INTERNATIONAL9},
    {"LANG1", KEYCODE_LANG1},
    {"LANG2", KEYCODE_LANG2},
    {"LANG3", KEYCODE_LANG3},
    {"LANG4", KEYCODE_LANG4},
    {"LANG5", KEYCODE_LANG5},
    {"LANG6", KEYCODE_LANG6},
    {"LANG7", KEYCODE_LANG7},
    {"LANG8", KEYCODE_LANG8},
    {"LANG9", KEYCODE_LANG9},
    {"ALT_ERASE", KEYCODE_ALTERASE},
    {"SYS_REQ", KEYCODE_SYSREQ},
    {"CANCEL", KEYCODE_CANCEL},
    {"CLEAR", KEYCODE_CLEAR},
    {"PRIOR", KEYCODE_PRIOR},
    {"RETURN2", KEYCODE_RETURN2},
    {"SEPARATOR", KEYCODE_SEPARATOR},
    {"OUT", KEYCODE_OUT},
    {"OPER", KEYCODE_OPER},
    {"CLEAR_AGAIN", KEYCODE_CLEARAGAIN},
    {"CRSEL", KEYCODE_CRSEL},
    {"EXSEL", KEYCODE_EXSEL},
    {"KP_00", KEYCODE_KP_00},
    {"KP_000", KEYCODE_KP_000},
    {"THOUSANDS_SEPARATOR", KEYCODE_THOUSANDSSEPARATOR},
    {"DECIMAL_SEPARATOR", KEYCODE_DECIMALSEPARATOR},
    {"CURRENCY_UNIT", KEYCODE_CURRENCYUNIT},
    {"CURRENCY_SUBUNIT", KEYCODE_CURRENCYSUBUNIT},
    {"KP_LEFT_PAREN", KEYCODE_KP_LEFTPAREN},
    {"KP_RIGHT_PAREN", KEYCODE_KP_RIGHTPAREN},
    {"KP_LEFT_BRACE", KEYCODE_KP_LEFTBRACE},
    {"KP_RIGHT_BRACE", KEYCODE_KP_RIGHTBRACE},
    {"KP_TAB", KEYCODE_KP_TAB},
    {"KP_BACKSPACE", KEYCODE_KP_BACKSPACE},
    {"KP_A", KEYCODE_KP_A},
    {"KP_B", KEYCODE_KP_B},
    {"KP_C", KEYCODE_KP_C},
    {"KP_D", KEYCODE_KP_D},
    {"KP_E", KEYCODE_KP_E},
    {"KP_F", KEYCODE_KP_F},
    {"KP_XOR", KEYCODE_KP_XOR},
    {"KP_POWER", KEYCODE_KP_POWER},
    {"KP_PERCENT", KEYCODE_KP_PERCENT},
    {"KP_LESS", KEYCODE_KP_LESS},
    {"KP_GREATER", KEYCODE_KP_GREATER},
    {"KP_AMPERSAND", KEYCODE_KP_AMPERSAND},
    {"KP_DBL_AMPERSAND", KEYCODE_KP_DBLAMPERSAND},
    {"KP_VERTICAL_BAR", KEYCODE_KP_VERTICALBAR},
    {"KP_DBL_VERTICAL_BAR", KEYCODE_KP_DBLVERTICALBAR},
    {"KP_COLON", KEYCODE_KP_COLON},
    {"KP_HASH", KEYCODE_KP_HASH},
    {"KP_SPACE", KEYCODE_KP_SPACE},
    {"KP_AT", KEYCODE_KP_AT},
    {"KP_EXCLAM", KEYCODE_KP_EXCLAM},
    {"KP_MEM_STORE", KEYCODE_KP_MEMSTORE},
    {"KP_MEM_RECALL", KEYCODE_KP_MEMRECALL},
    {"KP_MEM_CLEAR", KEYCODE_KP_MEMCLEAR},
    {"KP_MEM_ADD", KEYCODE_KP_MEMADD},
    {"KP_MEM_SUBTRACT", KEYCODE_KP_MEMSUBTRACT},
    {"KP_MEM_MULTIPLY", KEYCODE_KP_MEMMULTIPLY},
    {"KP_MEM_DIVIDE", KEYCODE_KP_MEMDIVIDE},
    {"KP_PLUS_MINUS", KEYCODE_KP_PLUSMINUS},
    {"KP_CLEAR", KEYCODE_KP_CLEAR},
    {"KP_CLEAR_ENTRY", KEYCODE_KP_CLEARENTRY},
    {"KP_BINARY", KEYCODE_KP_BINARY},
    {"KP_OCTAL", KEYCODE_KP_OCTAL},
    {"KP_DECIMAL", KEYCODE_KP_DECIMAL},
    {"KP_HEXADECIMAL", KEYCODE_KP_HEXADECIMAL},
    {"LEFT_CTRL", KEYCODE_LCTRL},
    {"LEFT_SHIFT", KEYCODE_LSHIFT},
    {"LEFT_ALT", KEYCODE_LALT},
    {"LEFT_GUI", KEYCODE_LGUI},
    {"RIGHT_CTRL", KEYCODE_RCTRL},
    {"RIGHT_SHIFT", KEYCODE_RSHIFT},
    {"RIGHT_ALT", KEYCODE_RALT},
    {"RIGHT_GUI", KEYCODE_RGUI},
    {"MODE", KEYCODE_MODE},
    {"AUDIO_NEXT", KEYCODE_AUDIONEXT},
    {"AUDIO_PREV", KEYCODE_AUDIOPREV},
    {"AUDIO_STOP", KEYCODE_AUDIOSTOP},
    {"AUDIO_PLAY", KEYCODE_AUDIOPLAY},
    {"AUDIO_MUTE", KEYCODE_AUDIOMUTE},
    {"MEDIA_SELECT", KEYCODE_MEDIASELECT},
    {"WWW", KEYCODE_WWW},
    {"MAIL", KEYCODE_MAIL},
    {"CALCULATOR", KEYCODE_CALCULATOR},
    {"COMPUTER", KEYCODE_COMPUTER},
    {"AC_SEARCH", KEYCODE_AC_SEARCH},
    {"AC_HOME", KEYCODE_AC_HOME},
    {"AC_BACK", KEYCODE_AC_BACK},
    {"AC_FORWARD", KEYCODE_AC_FORWARD},
    {"AC_STOP", KEYCODE_AC_STOP},
    {"AC_REFRESH", KEYCODE_AC_REFRESH},
    {"AC_BOOKMARKS", KEYCODE_AC_BOOKMARKS},
    {"BRIGHTNESS_DOWN", KEYCODE_BRIGHTNESSDOWN},
    {"BRIGHTNESS_UP", KEYCODE_BRIGHTNESSUP},
    {"DISPLAY_SWITCH", KEYCODE_DISPLAYSWITCH},
    {"KBD_ILLUM_TOGGLE", KEYCODE_KBDILLUMTOGGLE},
    {"KBD_ILLUM_DOWN", KEYCODE_KBDILLUMDOWN},
    {"KBD_ILLUM_UP", KEYCODE_KBDILLUMUP},
    {"EJECT", KEYCODE_EJECT},
    {"SLEEP", KEYCODE_SLEEP},
    {"APP1", KEYCODE_APP1},
    {"APP2", KEYCODE_APP2},
    {"AUDIO_REWIND", KEYCODE_AUDIOREWIND},
    {"AUDIO_FAST_FORWARD", KEYCODE_AUDIOFASTFORWARD},
    {NULL, 0}
};

static int keyboard_scan_code_table(lua_State*L) {
    lua_newtable(L);

    const struct luaL_Field* scancode;
    for (scancode = keyboard_scancodes; scancode->name; scancode++) {
        lua_pushinteger(L, scancode->value);
        lua_setfield(L, -2, scancode->name);
    }

    return 1;
}

static const struct luaL_Reg modules_keyboard_functions[] = {
    {"key", modules_keyboard_key_get},
    {NULL, NULL}
};

int luaopen_keyboard(lua_State* L) {
    luaL_newlib(L, modules_keyboard_functions);

    keyboard_scan_code_table(L);
    lua_setfield(L, -2, "scancodes");

    return 1;
}
