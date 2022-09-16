#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "assets.h"

#include "../assets.h"

int bindings_assets_get_texture(lua_State* L) {
    const char* texture_name = lua_tostring(L, 1);
    texture_t* texture = assets_get_texture(texture_name);

    if (texture) {
        lua_pushlightuserdata(L, texture);
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
