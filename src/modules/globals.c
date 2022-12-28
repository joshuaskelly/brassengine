/**
 * Global functions.
 * @module globals
 */
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "../time.h"

#include "globals.h"

/**
 * Prints given object to console.
 * @function print
 * @param arg Object to print to console
 */
static int api_print(lua_State* L) {
    const char* message = luaL_tolstring(L, -1, NULL);
    printf("%s\n", message);

    lua_pop(L, -1);

    return 0;
}

/**
 * Gets time since last frame.
 * @function delta_time
 * @return Time elapsed in milliseconds.
 */
static int api_get_delta_time(lua_State* L) {
    double delta_time = time_delta_time();
    lua_pushnumber(L, delta_time);

    return 1;
}

/**
 * Gets time since app launch.
 * @function time
 * @return Time elapsed in milliseconds.
 */
static int api_get_time_since_init(lua_State* L) {
    double time = time_since_init();
    lua_pushnumber(L, time);

    return 1;
}

void luaL_openglobals(lua_State* L) {
    lua_register(L, "print", api_print);
    lua_register(L, "delta_time", api_get_delta_time);
    lua_register(L, "time", api_get_time_since_init);
}
