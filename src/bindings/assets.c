#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "assets.h"

#include "../assets.h"

/**
 * Get texture for given filename
 *
 * @param filename Name of texture asset to look for
 * @return Texture userdata if found, nil otherwise.
 */
static int bindings_assets_get_texture(lua_State* L) {
    const char* texture_name = luaL_checkstring(L, 1);
    texture_t* texture = assets_get_texture(texture_name);

    if (texture) {
        texture_t** tp = (texture_t**)lua_newuserdata(L, sizeof(texture_t*));
        *tp = texture;
        luaL_setmetatable(L, "texture");
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static const struct luaL_Reg module_functions[] = {
    {"get_texture", bindings_assets_get_texture},
    {NULL, NULL}
};

int open_assets_module(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
