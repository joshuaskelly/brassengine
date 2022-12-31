/**
 * Module for playing sounds.
 * @module sound
 */
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "../platform.h"

#include "sound.h"

sound_t* luaL_checksound(lua_State* L, int index) {
    sound_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    handle = (sound_t**)luaL_checkudata(L, index, "sound");

    if (!handle) {
        luaL_typeerror(L, index, "sound");
    }

    return *handle;
}

int lua_pushsound(lua_State* L, sound_t* sound) {
    sound_t** handle = (sound_t**)lua_newuserdata(L, sizeof(sound_t*));
    *handle = sound;
    luaL_setmetatable(L, "sound");

    return 1;
}

static int sound_gc(lua_State* L) {
    // TODO: Only free up dynamic textures, not ones acquire from assets
    //sound_t** sound = lua_touserdata(L, 1);
    //sounds_sound_free(*texture);

    return 0;
}

static const struct luaL_Reg sound_methods[] = {
    {NULL, NULL}
};

/**
 * Plays given sound.
 * @function play
 * @param sound Sound to play.
 */
static int play_sound(lua_State* L) {
    sound_t* sound = luaL_checksound(L, 1);
    platform_play_sound(sound);

    return 0;
}

static const struct luaL_Reg module_functions[] = {
    {"play", play_sound},
    {NULL, NULL}
};

int luaopen_sound(lua_State* L) {
    luaL_newlib(L, module_functions);

// Push texture userdata metatable
    luaL_newmetatable(L, "sound");
    luaL_newlib(L, sound_methods);
    lua_setfield(L, -2, "__index");

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, sound_gc);
    lua_settable(L, -3);

    lua_pop(L, 1);

    return 1;
}
