/**
 * Module for getting information about the runtime environment.
 * @module statistics
 */

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "../script.h"
#include "../time.h"

#include "statistics.h"

/**
 * Gets time used by _draw function call in milliseconds
 * @function draw_time
 * @treturn integer Time in milliseconds
 */
static int modules_statistics_draw_time_get(lua_State* L) {
    double draw_time = script_draw_time_get();
    lua_pushnumber(L, draw_time);

    return 1;
}

/**
 * Gets time used by _update function call in milliseconds
 * @function update_time
 * @treturn integer Time in milliseconds
 */
static int modules_statistics_update_time_get(lua_State* L) {
    double update_time = script_update_time_get();
    lua_pushnumber(L, update_time);

    return 1;
}

/**
 * Gets current fps
 * @function fps
 * @treturn number Frames per second
 */
static int modules_statistics_fps_get(lua_State* L) {
    double fps = 1000.0 / time_delta_time();
    lua_pushnumber(L, fps);

    return 1;
}

static const struct luaL_Reg modules_statistics_functions[] = {
    {"draw_time", modules_statistics_draw_time_get},
    {"update_time", modules_statistics_update_time_get},
    {"fps", modules_statistics_fps_get},
    {NULL, NULL}
};

int luaopen_statistics(lua_State* L) {
    luaL_newlib(L, modules_statistics_functions);
    return 1;
}
