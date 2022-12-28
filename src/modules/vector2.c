#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "vector2.h"

mfloat_t* luaL_checkvector2(lua_State* L, int index) {
    mfloat_t* vector = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    vector = (mfloat_t*)luaL_checkudata(L, index, "vector2");

    if (!vector) {
        luaL_typeerror(L, index, "vector2");
    }

    return vector;
}

static int vector2_new(lua_State* L) {
    float x = (float)luaL_optnumber(L, 1, 0);
    float y = (float)luaL_optnumber(L, 2, 0);

    lua_pop(L, -1);

    mfloat_t* vector = (mfloat_t*)lua_newuserdata(L, sizeof(mfloat_t) * 2);
    vector[0] = x;
    vector[1] = y;

    luaL_setmetatable(L, "vector2");

    return 1;
}

static int vector2_equal(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);
    mfloat_t* v1 = luaL_checkvector2(L, 2);

    lua_pop(L, -1);

    bool result = vec2_is_equal(v0, v1);

    lua_pushboolean(L, result);

    return 1;
}

static int vector2_add(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);
    mfloat_t* v1 = luaL_checkvector2(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_add(result, v0, v1);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_subtract(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);
    mfloat_t* v1 = luaL_checkvector2(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_subtract(result, v0, v1);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_multiply(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);

    // Scalar multiplication
    if (lua_isnumber(L, 2)) {
        float f = luaL_checknumber(L, 2);

        lua_pop(L, -1);

        mfloat_t result[VEC2_SIZE];
        vec2_multiply_f(result, v0, f);

        lua_pushnumber(L, result[0]);
        lua_pushnumber(L, result[1]);
        vector2_new(L);

        return 1;
    }

    // Component-wise multiplication
    mfloat_t* v1 = luaL_checkvector2(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_multiply(result, v0, v1);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_divide(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);

    // Scalar division
    if (lua_isnumber(L, 2)) {
        float f = luaL_checknumber(L, 2);

        lua_pop(L, -1);

        mfloat_t result[VEC2_SIZE];
        vec2_divide_f(result, v0, f);

        lua_pushnumber(L, result[0]);
        lua_pushnumber(L, result[1]);
        vector2_new(L);

        return 1;
    }

    // Component-wise division
    mfloat_t* v1 = luaL_checkvector2(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_divide(result, v0, v1);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_snap(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);

    // Scalar snapping
    if (lua_isnumber(L, 2)) {
        float f = luaL_checknumber(L, 2);

        lua_pop(L, -1);

        mfloat_t result[VEC2_SIZE];
        vec2_snap_f(result, v0, f);

        lua_pushnumber(L, result[0]);
        lua_pushnumber(L, result[1]);
        vector2_new(L);

        return 1;
    }

    // Component-wise snapping
    mfloat_t* v1 = luaL_checkvector2(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_snap(result, v0, v1);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_negative(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_negative(result, v0);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}


static int vector2_abs(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_abs(result, v0);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_floor(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_floor(result, v0);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}


static int vector2_ceil(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_ceil(result, v0);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_round(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_round(result, v0);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_max(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);
    mfloat_t* v1 = luaL_checkvector2(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_max(result, v0, v1);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_min(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);
    mfloat_t* v1 = luaL_checkvector2(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_min(result, v0, v1);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_clamp(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);
    mfloat_t* v1 = luaL_checkvector2(L, 2);
    mfloat_t* v2 = luaL_checkvector2(L, 3);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_clamp(result, v0, v1, v2);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_normalize(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_normalize(result, v0);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_dot(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);
    mfloat_t* v1 = luaL_checkvector2(L, 2);

    lua_pop(L, -1);

    float result = vec2_dot(v0, v1);

    lua_pushnumber(L, result);

    return 1;
}

static int vector2_project(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);
    mfloat_t* v1 = luaL_checkvector2(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_project(result, v0, v1);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_slide(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);
    mfloat_t* v1 = luaL_checkvector2(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_slide(result, v0, v1);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_reflect(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);
    mfloat_t* v1 = luaL_checkvector2(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_reflect(result, v0, v1);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_tangent(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_tangent(result, v0);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_rotate(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);
    float f = luaL_checknumber(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_rotate(result, v0, f);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_lerp(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);
    mfloat_t* v1 = luaL_checkvector2(L, 2);
    float f = luaL_checknumber(L, 3);

    lua_pop(L, -1);

    mfloat_t result[VEC2_SIZE];
    vec2_lerp(result, v0, v1, f);

    lua_pushnumber(L, result[0]);
    lua_pushnumber(L, result[1]);
    vector2_new(L);

    return 1;
}

static int vector2_angle(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);

    lua_pop(L, -1);

    float result = vec2_angle(v0);

    lua_pushnumber(L, result);

    return 1;
}

static int vector2_length(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);

    lua_pop(L, -1);

    float result = vec2_length(v0);

    lua_pushnumber(L, result);

    return 1;
}

static int vector2_length_squared(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);

    lua_pop(L, -1);

    float result = vec2_length_squared(v0);

    lua_pushnumber(L, result);

    return 1;
}

static int vector2_distance(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);
    mfloat_t* v1 = luaL_checkvector2(L, 2);

    lua_pop(L, -1);

    float result = vec2_distance(v0, v1);

    lua_pushnumber(L, result);

    return 1;
}

static int vector2_distance_squared(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector2(L, 1);
    mfloat_t* v1 = luaL_checkvector2(L, 2);

    lua_pop(L, -1);

    float result = vec2_distance_squared(v0, v1);

    lua_pushnumber(L, result);

    return 1;
}

static const struct luaL_Reg module_functions[] = {
    {"new", vector2_new},
    {"snap", vector2_snap},
    {"abs", vector2_abs},
    {"floor", vector2_floor},
    {"ceil", vector2_ceil},
    {"round", vector2_round},
    {"max", vector2_max},
    {"min", vector2_min},
    {"clamp", vector2_clamp},
    {"normalize", vector2_normalize},
    {"dot", vector2_dot},
    {"project", vector2_project},
    {"slide", vector2_slide},
    {"reflect", vector2_reflect},
    {"tangent", vector2_tangent},
    {"rotate", vector2_rotate},
    {"lerp", vector2_lerp},
    {"angle", vector2_angle},
    {"length", vector2_length},
    {"length_squared", vector2_length_squared},
    {"distance", vector2_distance},
    {"distance_squared", vector2_distance_squared},
    {NULL, NULL}
};

static int vector2_meta_index(lua_State* L) {
    mfloat_t* vector = luaL_checkvector2(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_pop(L, -1);

    if (strcmp(key, "x") == 0) {
        lua_pushnumber(L, vector[0]);
    }
    else if (strcmp(key, "y") == 0) {
        lua_pushnumber(L, vector[1]);
    }
    else if (strcmp(key, "length") == 0) {
        lua_pushnumber(L, vec2_length(vector));
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static int vector2_meta_newindex(lua_State* L) {
    mfloat_t* vector = luaL_checkvector2(L, 1);
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
    {"__sub", vector2_subtract},
    {"__mul", vector2_multiply},
    {"__div", vector2_divide},
    {"__unm", vector2_negative},
    {"__eq", vector2_equal},
    {NULL, NULL}
};

int luaopen_vector2(lua_State* L) {
    luaL_newlib(L, module_functions);

    luaL_newmetatable(L, "vector2");
    luaL_setfuncs(L, meta_functions, 0);

    lua_pop(L, 1);

    return 1;
}
