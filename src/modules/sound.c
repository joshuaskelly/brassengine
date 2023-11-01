/**
 * Module for playing sounds.
 * @module sound
 */
#include <stdbool.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "sound.h"

#include "../sounds.h"

sound_t* luaL_checksound(lua_State* L, int index) {
    sound_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);

    handle = (sound_t**)luaL_testudata(L, index, "sound_nogc");
    if (!handle) {
        handle = (sound_t**)luaL_checkudata(L, index, "sound");
    }

    return *handle;
}

int lua_pushsound(lua_State* L, sound_t* sound) {
    sound_t** handle = (sound_t**)lua_newuserdata(L, sizeof(sound_t*));
    *handle = sound;
    luaL_setmetatable(L, "sound_nogc");

    return 1;
}

static int sound_gc(lua_State* L) {
    sound_t** sound = lua_touserdata(L, 1);
    sounds_sound_free(*sound);
    *sound = NULL;

    return 0;
}

static int sound_meta_index(lua_State* L) {
    sound_t* sound = luaL_checksound(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_settop(L, 0);

    if (strcmp(key, "pcm") == 0) {
        lua_newtable(L);

        for (int i = 0; i < sound->frame_count * sound->channel_count; i++) {
            lua_pushinteger(L, i + 1);
            lua_pushinteger(L, (sample_t)sound->pcm[i]);
            lua_settable(L, -3);
        }
    }
    else if (strcmp(key, "frame_count") == 0) {
        lua_pushinteger(L, sound->frame_count);
    }
    else if (strcmp(key, "channel_count") == 0) {
        lua_pushinteger(L, sound->channel_count);
    }
    else {
        // Check module fields. This enables usage of the colon operator.
        luaL_requiref(L, "sound", NULL, false);
        if (lua_type(L, -1) == LUA_TTABLE) {
            lua_getfield(L, -1, key);
        }
        else {
            lua_pushnil(L);
        }
    }

    return 1;
}

static int sound_meta_newindex(lua_State* L) {
    sound_t* sound = luaL_checksound(L, 1);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "pcm") == 0) {
        size_t sample_count = sound->frame_count * sound->channel_count;
        size_t table_size = lua_rawlen(L, 3);

        if (table_size == sample_count) {
            for (int i = 0; i < sample_count; i++) {
                int index = i + 1;
                lua_pushinteger(L, index);
                lua_gettable(L, 3);

                sound->pcm[i] = (sample_t)luaL_checknumber(L, -1);

                lua_pop(L, 0);
            }

            lua_settop(L, 0);
        }
        else {
            luaL_error(L, "BAD!");
        }
    }
    else {
        luaL_error(L, "attempt to index a sound value");
    }

    return 0;
}

static const struct luaL_Reg meta_functions[] = {
    {"__index", sound_meta_index},
    {"__newindex", sound_meta_newindex},
    {NULL, NULL}
};

/**
 * Create new sound.
 * @function new
 * @param frame_count Total number of PCM frames.
 * @return @{sound}
 */
static int new_sound(lua_State* L) {
    int frame_count = (int)luaL_checknumber(L, 1);

    sound_t** handle = (sound_t**)lua_newuserdata(L, sizeof(sound_t*));
    *handle = sounds_sound_new(frame_count, 1, NULL);
    luaL_setmetatable(L, "sound");

    return 1;
}

/**
 * @type sound
 */

/**
 * Returns a copy of this sound.
 * @function copy
 * @return @{sound}
 */
static int copy_sound(lua_State* L) {
    sound_t* source = luaL_checksound(L, 1);

    lua_pop(L, -1);

    sound_t** handle = (sound_t**)lua_newuserdata(L, sizeof(sound_t*));
    *handle = sounds_sound_copy(source);
    luaL_setmetatable(L, "sound");

    return 1;
}

static int set_sound_frame(lua_State* L) {
    sound_t* sound = luaL_checksound(L, 1);
    int index = (int)luaL_checknumber(L, 2);

    int frame_size = sound->channel_count;
    int offset = index * frame_size;

    size_t table_size = lua_rawlen(L, 3);

    if (table_size == frame_size) {
        for (int i = 0; i < frame_size; i++) {
            int ii = i + 1;
            lua_pushinteger(L, ii);
            lua_gettable(L, 3);

            sound->pcm[i + offset] = (sample_t)luaL_checknumber(L, -1);

            lua_pop(L, 1);
        }

        lua_pop(L, 1);
    }
    else {
            luaL_error(L, "BAD!");
    }

    return 0;
}

static int get_sound_frame(lua_State* L) {
    sound_t* sound = luaL_checksound(L, 1);
    int index = (int)luaL_checknumber(L, 2);

    int frame_size = sound->channel_count;
    int offset = index * frame_size;

    for (int i = 0; i < frame_size; i++) {
        lua_pushinteger(L, sound->pcm[offset + i]);
    }

    return frame_size;
}

/**
 * Plays sound.
 * @function play
 * @param channel Channel to play sound on. (optional)
 */
static int play_sound(lua_State* L) {
    sound_t* sound = luaL_checksound(L, 1);
    int channel = luaL_optnumber(L, 2, -1);
    sounds_sound_play(sound, channel);

    return 0;
}

/**
 * An array copy of PCM data.
 * @field pcm
 */

/**
 * Total number of frames
 * @field frame_count (read-only)
 */

static const struct luaL_Reg module_functions[] = {
    {"new", new_sound},
    {"copy", copy_sound},
    {"set_frame", set_sound_frame},
    {"get_frame", get_sound_frame},
    {"play", play_sound},
    {NULL, NULL}
};

int luaopen_sound(lua_State* L) {
    luaL_newlib(L, module_functions);

    // Push sound userdata metatable
    luaL_newmetatable(L, "sound");
    luaL_setfuncs(L, meta_functions, 0);

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, sound_gc);
    lua_settable(L, -3);

    lua_pop(L, 1);

    // Push sound userdata metatable
    luaL_newmetatable(L, "sound_nogc");
    luaL_setfuncs(L, meta_functions, 0);

    lua_pop(L, 1);

    return 1;
}
