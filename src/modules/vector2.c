#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "vector2.h"

mfloat_t* luaL_checkvector(lua_State* L, int index) {
    mfloat_t* vector = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    vector = (mfloat_t*)luaL_checkudata(L, index, "vector2");

    if (!vector) {
        luaL_typeerror(L, index, "vector2");
    }

    return vector;
}

static int vector2_new(lua_State* L) {
    float x = (float)luaL_checknumber(L, 1);
    float y = (float)luaL_checknumber(L, 2);

    lua_pop(L, -1);

    mfloat_t* vector = (mfloat_t*)lua_newuserdata(L, sizeof(mfloat_t) * 2);
    vector[0] = x;
    vector[1] = y;

    luaL_setmetatable(L, "vector2");

    return 1;
}

static int vector2_add(lua_State* L) {
    mfloat_t* a = luaL_checkvector(L, 1);
    mfloat_t* b = luaL_checkvector(L, 2);

    lua_pop(L, -1);

    float x = a[0] + b[0];
    float y = a[1] + b[1];

    lua_pushnumber(L, x);
    lua_pushnumber(L, y);

    vector2_new(L);

    return 1;
}

static const struct luaL_Reg module_functions[] = {
    {"new", vector2_new},
    {"add", vector2_add},
    {NULL, NULL}
};

static int vector2_meta_index(lua_State* L) {
    mfloat_t* vector = luaL_checkvector(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_pop(L, -1);

    if (strcmp(key, "x") == 0) {
        lua_pushnumber(L, vector[0]);
    }
    else if (strcmp(key, "y") == 0) {
        lua_pushnumber(L, vector[1]);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static int vector2_meta_newindex(lua_State* L) {
    mfloat_t* vector = luaL_checkvector(L, 1);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "x") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        vector[0] = value;
    }
    else if (strcmp(key, "y") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        vector[1] = value;
    }
    else {
        luaL_error(L, "attempt to index a vector2 value");
    }

    lua_pop(L, -1);

    return 0;
}

static const struct luaL_Reg meta_functions[] = {
    {"__index", vector2_meta_index},
    {"__newindex", vector2_meta_newindex},
    {"__add", vector2_add},
    {NULL, NULL}
};

int luaopen_vector2(lua_State* L) {
    luaL_newlib(L, module_functions);

    luaL_newmetatable(L, "vector2");
    luaL_setfuncs(L, meta_functions, 0);

    lua_pop(L, 1);

    return 1;
}
