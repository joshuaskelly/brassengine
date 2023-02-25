/**
 * Module for interacting with the display.
 * @module display
 */
#include <stddef.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "display.h"

#include "../configuration.h"
#include "../graphics.h"
#include "../platform.h"

/**
 * Set display size.
 * @function set_size
 * @param width Display width
 * @param height Display height
 */
static int bindings_display_set_window_size(lua_State* L) {
    int width = (int)luaL_checknumber(L, 1);
    int height = (int)luaL_checknumber(L, 2);

    lua_pop(L, -1);

    platform_display_set_size(width, height);

    return 0;
}

/**
 * Set display fullscreen.
 * @function set_fullscreen
 * @param fullscreen True if to set to fullscreen, False for windowed.
 */
static int bindings_display_set_window_fullscreen(lua_State* L) {
    bool fullscreen = (bool)lua_toboolean(L, 1);

    lua_pop(L, -1);

    platform_display_set_fullscreen(fullscreen);

    return 0;
}

/**
 * Set display title.
 * @function set_title
 * @param title Title to set
 */
static int bindings_display_set_title(lua_State* L) {
    const char* title = (char*)lua_tostring(L, 1);

    lua_pop(L, -1);

    platform_display_set_title(title);

    return 0;
}

static const struct luaL_Reg module_functions[] = {
    {"set_size", bindings_display_set_window_size},
    {"set_fullscreen", bindings_display_set_window_fullscreen},
    {"set_title", bindings_display_set_title},
    {NULL, NULL}
};

int luaopen_display(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
