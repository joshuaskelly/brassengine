/**
 * Module for working with gif data.
 * @module gif
 */
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "gif.h"
#include "texture.h"

#include "../assets.h"

/**
 * Save frames as an animated GIF. Result will be encoded at 50 fps.
 * @function save
 * @tparam string filename Name of GIF to save.
 * @tparam {texture.texture,...} frames Array of texture userdata.
 */
static int modules_gif_save(lua_State* L) {
    const char* filename = luaL_checkstring(L, 1);

    // Array of textures
    if (lua_istable(L, 2)) {
        size_t frame_count = lua_rawlen(L, 2);
        texture_t* frames[frame_count];

        for (int i = 0; i < frame_count; i++) {
            int index = i + 1;
            lua_pushinteger(L, index);
            lua_gettable(L, 2);

            if (lua_type(L, -1) == LUA_TNIL) break;

            frames[i] = luaL_checktexture(L, -1);

            lua_pop(L, 1);
        }

        lua_settop(L, 0);

        assets_gif_save(filename, frame_count, frames);
    }
    // Single texture
    else {
        texture_t* frame = luaL_checktexture(L, 2);

        texture_t* frames[] = {
            frame
        };

        lua_settop(L, 0);

        assets_gif_save(filename, 1, frames);
    }

    return 0;
}

static const struct luaL_Reg modules_gif_functions[] = {
    {"save", modules_gif_save},
    {NULL, NULL}
};

int luaopen_gif(lua_State* L) {
    luaL_newlib(L, modules_gif_functions);

    return 1;
}
