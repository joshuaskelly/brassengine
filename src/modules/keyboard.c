#include <ctype.h>
#include <stdbool.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "keyboard.h"

#include "../input.h"

/**
 * Check if given key is down.
 *
 * @param int|string Key code or symbol to check.
 *
 * @return True if key is down.
 */
static int bindings_get_keyboard_key(lua_State* L) {
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

static const struct luaL_Reg module_functions[] = {
    {"key", bindings_get_keyboard_key},
    {NULL, NULL}
};

int luaopen_keyboard(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
