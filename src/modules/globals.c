/**
 * Global functions.
 * @module globals
 */
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "../console.h"
#include "../core.h"
#include "../event.h"
#include "../log.h"
#include "../time.h"

#include "globals.h"

/**
 * Prints given object to console.
 * @function print
 * @param arg Object to print to console
 */
static int api_print(lua_State* L) {
    int n = lua_gettop(L);
    for (int i = 1; i <= n; i++) {
        const char* message = luaL_tolstring(L, i, NULL);

        if (i > 1) {
            // TODO: Tabs?
        }

        log_info(message);
        lua_pop(L, 1);
    }

    return 0;
}

static int api_clear(lua_State* L) {
    console_buffer_clear();
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

/**
 * Quits app.
 * @function quit
 */
static int api_quit(lua_State* L) {
    event_t event;
    event.type = EVENT_QUIT;
    event_post(&event);

    return 0;
}

static int api_reload(lua_State* L) {
    event_t event;
    event.type = EVENT_RELOAD;
    event_post(&event);

    return 0;
}

void luaL_openglobals(lua_State* L) {
    lua_register(L, "print", api_print);
    lua_register(L, "clear", api_clear);
    lua_register(L, "delta_time", api_get_delta_time);
    lua_register(L, "time", api_get_time_since_init);
    lua_register(L, "quit", api_quit);
    lua_register(L, "reload", api_reload);
}
