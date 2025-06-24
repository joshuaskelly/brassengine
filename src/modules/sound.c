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

static int modules_sound_gc(lua_State* L) {
    sound_t** sound = lua_touserdata(L, 1);
    sounds_sound_free(*sound);
    *sound = NULL;

    return 0;
}

static int modules_sound_meta_index(lua_State* L) {
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

static int modules_sound_meta_newindex(lua_State* L) {
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

static const struct luaL_Reg modules_sound_meta_functions[] = {
    {"__index", modules_sound_meta_index},
    {"__newindex", modules_sound_meta_newindex},
    {NULL, NULL}
};

/**
 * Create new sound.
 * @function new
 * @tparam integer frame_count Total number of PCM frames.
 * @treturn sound
 */
static int modules_sound_new(lua_State* L) {
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
 * Plays sound.
 * @function play
 * @tparam integer channel Channel to play sound on. (optional)
 */
static int modules_sound_play(lua_State* L) {
    sound_t* sound = luaL_checksound(L, 1);

    int arg = 2;

    int channel = -1;
    if (lua_isinteger(L, arg)) {
        channel = luaL_checkinteger(L, arg);
        arg++;
    }

    bool looping = false;
    if (lua_isboolean(L, arg)) {
        looping = lua_toboolean(L, arg);
        arg++;
    }

    sounds_sound_play(sound, channel, looping);

    return 0;
}

/**
 * Returns a copy of this sound.
 * @function copy
 * @treturn sound
 */
static int modules_sound_copy(lua_State* L) {
    sound_t* source = luaL_checksound(L, 1);

    lua_pop(L, -1);

    sound_t** handle = (sound_t**)lua_newuserdata(L, sizeof(sound_t*));
    *handle = sounds_sound_copy(source);
    luaL_setmetatable(L, "sound");

    return 1;
}

/**
 * Sets PCM data for given frame.
 * @function set_frame
 * @tparam integer index Frame index
 * @tparam integer value PCM data value
 */
static int modules_sound_frame_set(lua_State* L) {
    sound_t* sound = luaL_checksound(L, 1);
    int index = (int)luaL_checknumber(L, 2);
    sample_t value = (sample_t)luaL_checknumber(L, 3);

    lua_pop(L, -1);

    int frame_size = sound->channel_count;
    int offset = index * frame_size;

    sound->pcm[offset] = value;

    return 0;
}

/**
 * Gets PCM data for given frame.
 * @function get_frame
 * @tparam integer index Frame index
 * @treturn integer PCM data
 */
static int modules_sound_frame_get(lua_State* L) {
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
 * An array copy of PCM data.
 * @tfield {integer,...} pcm
 */

/**
 * Total number of frames
 * @tfield integer frame_count (read-only)
 */

static const struct luaL_Reg modules_sound_functions[] = {
    {"new", modules_sound_new},
    {"copy", modules_sound_copy},
    {"set_frame", modules_sound_frame_set},
    {"get_frame", modules_sound_frame_get},
    {"play", modules_sound_play},
    {NULL, NULL}
};

int luaopen_sound(lua_State* L) {
    luaL_newlib(L, modules_sound_functions);

    // Push sound userdata metatable
    luaL_newmetatable(L, "sound");
    luaL_setfuncs(L, modules_sound_meta_functions, 0);

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, modules_sound_gc);
    lua_settable(L, -3);

    lua_pop(L, 1);

    // Push sound userdata metatable
    luaL_newmetatable(L, "sound_nogc");
    luaL_setfuncs(L, modules_sound_meta_functions, 0);

    lua_pop(L, 1);

    return 1;
}
