/**
 * Module for getting mouse input.
 * @module mouse
 */
#include <stdbool.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "mouse.h"

#include "../input.h"
#include "../platform.h"

/**
 * Check if given button is down.
 * @function button
 * @param button Button index to check. Primary button is 1, middle button is 2,
 * secondary button is 3.
 * @return True if button is down, false otherwise
 */
static int bindings_get_mouse_button(lua_State* L) {
    int button = luaL_checkinteger(L, 1);

    lua_pop(L, -1);

    bool is_pressed = input_mouse_is_button_pressed(button);
    lua_pushboolean(L, is_pressed);

    return 1;
}

/**
 * Get current mouse position.
 * @function position
 * @return Mouse coords as x, y
 */
static int bindings_get_mouse_position(lua_State* L) {
    int x = 0;
    int y = 0;

    input_mouse_position(&x, &y);

    lua_pushinteger(L, x);
    lua_pushinteger(L, y);

    return 2;
}

/**
 * Set mouse grab state.
 * @function set_grabbed
 * @param grabbed True if mouse should be constrained to window.
 */
static int bindings_set_mouse_grabbed(lua_State* L) {
    bool grabbed = lua_toboolean(L, 1);

    lua_settop(L, 0);

    platform_mouse_set_grabbed(grabbed);

    return 0;
}

/**
 * Get mouse grab state.
 *
 * @return True if mouse is grabbed, false otherwise.
 */
static int bindings_get_mouse_grabbed(lua_State* L) {
    bool grabbed = platform_mouse_get_grabbed();

    lua_pushboolean(L, grabbed);

    return 1;
}

static const struct luaL_Reg module_functions[] = {
    {"button", bindings_get_mouse_button},
    {"position", bindings_get_mouse_position},
    {"set_grabbed", bindings_set_mouse_grabbed},
    {"get_grabbed", bindings_get_mouse_grabbed},
    {NULL, NULL}
};

int luaopen_mouse(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
