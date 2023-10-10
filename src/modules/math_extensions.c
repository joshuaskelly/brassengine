#include <stdbool.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "../math.h"

static int module_math_clamp(lua_State* L) {
    float value = luaL_checknumber(L, 1);
    float min = luaL_checknumber(L, 2);
    float max = luaL_checknumber(L, 3);

    lua_pushnumber(L, clamp(value, min, max));

    return 1;
}

static int module_math_lerp(lua_State* L) {
    float a = luaL_checknumber(L, 1);
    float b = luaL_checknumber(L, 2);
    float t = luaL_checknumber(L, 3);

    lua_pushnumber(L, lerp(a, b, t));

    return 1;
}

static int module_math_noise(lua_State* L) {
    float x = luaL_checknumber(L, 1);
    float y = luaL_optnumber(L, 2, 0);
    float z = luaL_optnumber(L, 3, 0);

    lua_pushnumber(L, noise(x, y, z));

    return 1;
}

static int module_math_smoothstep(lua_State* L) {
    float a = luaL_checknumber(L, 1);
    float b = luaL_checknumber(L, 2);
    float t = luaL_checknumber(L, 3);

    lua_pushnumber(L, smoothstep(a, b, t));

    return 1;
}

static const struct luaL_Reg module_functions[] = {
    {"clamp", module_math_clamp},
    {"lerp", module_math_lerp},
    {"noise", module_math_noise},
    {"smoothstep", module_math_smoothstep},
    {NULL, NULL}
};

int luaopen_mathextensions(lua_State* L) {
    luaL_requiref(L, "math", NULL, false);

    const luaL_Reg *module;
    for (module = module_functions; module->func; module++) {
        lua_pushstring(L, module->name);
        lua_pushcfunction(L, module->func);
        lua_settable(L, -3);
    }

    lua_pop(L, 1);

    return 1;
}
