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
 * @tparam integer width Display width
 * @tparam integer height Display height
 */
static int bindings_display_window_size_set(lua_State* L) {
    int width = (int)luaL_checknumber(L, 1);
    int height = (int)luaL_checknumber(L, 2);

    lua_pop(L, -1);

    platform_display_size_set(width, height);

    return 0;
}

/**
 * Set display aspect ratio.
 * @function set_aspect
 * @tparam number aspect Pixel aspect ratio
 */
static int bindings_display_aspect_set(lua_State* L) {
    float aspect = (float)luaL_checknumber(L, 1);

    lua_pop(L, -1);

    config->display.aspect = aspect;

    return 0;
}

/**
 * Set display fullscreen.
 * @function set_fullscreen
 * @tparam boolean fullscreen True if to set to fullscreen, False for windowed.
 */
static int bindings_display_window_fullscreen_set(lua_State* L) {
    bool fullscreen = (bool)lua_toboolean(L, 1);

    lua_pop(L, -1);

    platform_display_fullscreen_set(fullscreen);

    return 0;
}

/**
 * Set display title.
 * @function set_title
 * @tparam string title Title to set
 */
static int bindings_display_title_set(lua_State* L) {
    const char* title = (char*)lua_tostring(L, 1);

    lua_pop(L, -1);

    platform_display_title_set(title);

    return 0;
}

static const struct luaL_Reg module_functions[] = {
    {"set_size", bindings_display_window_size_set},
    {"set_aspect", bindings_display_aspect_set},
    {"set_fullscreen", bindings_display_window_fullscreen_set},
    {"set_title", bindings_display_title_set},
    {NULL, NULL}
};

int luaopen_display(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
