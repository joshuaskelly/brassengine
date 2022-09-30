#include <stdbool.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "mouse.h"

#include "../input.h"

/**
 * Check if given button is down.
 *
 * @param int Button index to check. Primary button is 1, middle button is 2,
 * secondary button is 3.
 *
 * @return True if button is down, false otherwise
 */
int bindings_get_mouse_button(lua_State* L) {
    int button = luaL_checkinteger(L, 1);

    lua_pop(L, -1);

    bool is_pressed = input_mouse_is_button_pressed(button);
    lua_pushboolean(L, is_pressed);

    return 1;
}

/**
 * Get current mouse position.
 *
 * @return Mouse coords as x, y
 */
int bindings_get_mouse_position(lua_State* L) {
    int x = 0;
    int y = 0;

    input_mouse_position(&x, &y);

    lua_pushinteger(L, x);
    lua_pushinteger(L, y);

    return 2;
}

static const struct luaL_Reg module_functions[] = {
    {"button", bindings_get_mouse_button},
    {"position", bindings_get_mouse_position},
    {NULL, NULL}
};

int open_mouse_module(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
