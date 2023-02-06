/**
 * Module for getting information about the runtime environment.
 * @module statistics
 */

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "../script.h"

#include "statistics.h"

/**
 * Gets time used by _draw function call in milliseconds
 * @function draw_time
 * @return Time in milliseconds
 */
static int bindings_get_draw_time(lua_State* L) {
    double draw_time = script_draw_time_get();
    lua_pushnumber(L, draw_time);

    return 1;
}

/**
 * Gets time used by _update function call in milliseconds
 * @function update_time
 * @return Time in milliseconds
 */
static int bindings_get_update_time(lua_State* L) {
    double update_time = script_update_time_get();
    lua_pushnumber(L, update_time);

    return 1;
}

static const struct luaL_Reg module_functions[] = {
    {"draw_time", bindings_get_draw_time},
    {"update_time", bindings_get_update_time},
    {NULL, NULL}
};

int luaopen_statistics(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
