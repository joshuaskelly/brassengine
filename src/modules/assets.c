/**
 * Module for accessing assets.
 * @module assets
 */
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "assets.h"
#include "sound.h"
#include "texture.h"

#include "../assets.h"
#include "../sounds.h"

/**
 * Get texture for given filename
 * @function get_texture
 * @param filename Name of texture asset to look for
 * @return Texture userdata if found, nil otherwise.
 */
static int bindings_assets_get_texture(lua_State* L) {
    const char* texture_name = luaL_checkstring(L, 1);
    texture_t* texture = assets_get_texture(texture_name);

    if (texture) {
        lua_pushtexture(L, texture);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

/**
 * Get sound for given filename
 * @function get_sound
 * @param filename Name of sound asset to look for
 * @return Sound userdata if found, nil otherwise.
 */
static int bindings_assets_get_sound(lua_State* L) {
    const char* sound_name = luaL_checkstring(L, 1);
    sound_t* sound = assets_get_sound(sound_name);

    if (sound) {
        lua_pushsound(L, sound);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static const struct luaL_Reg module_functions[] = {
    {"get_texture", bindings_assets_get_texture},
    {"get_sound", bindings_assets_get_sound},
    {NULL, NULL}
};

int luaopen_assets(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
