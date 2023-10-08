#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "noise.h"

#include "../noise.h"

static int module_noise_perlin(lua_State* L) {
    float x = luaL_checknumber(L, 1);
    float y = luaL_optnumber(L, 2, 0);
    float z = luaL_optnumber(L, 3, 0);

    lua_pushnumber(L, noise_perlin(x, y, z));

    return 1;
}

static const struct luaL_Reg module_functions[] = {
    {"perlin", module_noise_perlin},
    {NULL, NULL}
};

int luaopen_noise(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
