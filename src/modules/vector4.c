/**
 * Module for working with vector4 data.
 * @module vector4
 */
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "vector4.h"

bool lua_isvector4(lua_State*L, int index) {
    void* p = luaL_testudata(L, index, "vector4");
    if (p == NULL) {
        p = luaL_testudata(L, index, "vector4_nogc");
    }

    return p != NULL;
}

mfloat_t* luaL_checkvector4(lua_State* L, int index) {
    mfloat_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);

    handle = (mfloat_t**)luaL_testudata(L, index, "vector4_nogc");
    if (!handle) {
        handle = (mfloat_t**)luaL_checkudata(L, index, "vector4");
    }

    return *handle;
}

int lua_newvector4(lua_State* L, float x, float y, float z, float w) {
    mfloat_t** handle = (mfloat_t**)lua_newuserdata(L, sizeof(mfloat_t*));
    mfloat_t* vector = (mfloat_t*)malloc(sizeof(mfloat_t) * VEC4_SIZE);
    vector[0] = x;
    vector[1] = y;
    vector[2] = z;
    vector[3] = w;
    *handle = vector;
    luaL_setmetatable(L, "vector4");

    return 1;
}

int lua_pushvector4(lua_State* L, mfloat_t* vector) {
    mfloat_t** handle = (mfloat_t**)lua_newuserdata(L, sizeof(mfloat_t*));
    *handle = vector;

    luaL_setmetatable(L, "vector4_nogc");

    return 1;
}

static int vector4_gc(lua_State* L) {
    mfloat_t** handle = lua_touserdata(L, 1);
    free(*handle);
    *handle = NULL;

    return 0;
}

/**
 * Vector4 class
 * @type vector4
 */

/**
 * X component of the vector
 * @tfield number x
 */

/**
 * Y component of the vector
 * @tfield number y
 */

/**
 * Z component of the vector
 * @tfield number z
 */

/**
 * W component of the vector
 * @tfield number w
 */

/**
 * Functions
 * @section Functions
 */

/**
 * Returns a new vector4
 * @function new
 * @tparam ?number x
 * @tparam ?number y
 * @tparam ?number z
 * @tparam ?number w
 * @treturn vector4
 */
static int modules_vector4_new(lua_State* L) {
    float x = (float)luaL_optnumber(L, 1, 0);
    float y = (float)luaL_optnumber(L, 2, 0);
    float z = (float)luaL_optnumber(L, 3, 0);
    float w = (float)luaL_optnumber(L, 4, 0);

    lua_settop(L, 0);

    lua_newvector4(L, x, y, z, w);

    return 1;
}

static int modules_vector4_equal(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);
    mfloat_t* v1 = luaL_checkvector4(L, 2);

    lua_settop(L, 0);

    bool result = vec4_is_equal(v0, v1);

    lua_pushboolean(L, result);

    return 1;
}

static int modules_vector4_zero(lua_State* L) {
    lua_newvector4(L, 0, 0, 0, 0);
    mfloat_t* vector = luaL_checkvector4(L, 1);
    vec4_zero(vector);

    return 1;
}

static int modules_vector4_one(lua_State* L) {
    lua_newvector4(L, 0, 0, 0, 0);
    mfloat_t* vector = luaL_checkvector4(L, 1);
    vec4_one(vector);

    return 1;
}

/**
 * Returns a vector made from the sign of it's components.
 * @function sign
 * @tparam vector4 v0
 * @treturn vector4
 */
static int modules_vector4_sign(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);
    mfloat_t result[VEC4_SIZE];
    vec4_sign(result, v0);
    lua_newvector4(L, result[0], result[1], result[2], result[3]);

    return 1;
}

static int modules_vector4_add(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);

    if (lua_isnumber(L, 2)) {
        mfloat_t f = luaL_checknumber(L, 2);

        lua_settop(L, 0);

        mfloat_t result[VEC4_SIZE];
        vec4_add_f(result, v0, f);

        lua_newvector4(L, result[0], result[1], result[2], result[3]);
    }
    else {
        mfloat_t* v1 = luaL_checkvector4(L, 2);

        lua_settop(L, 0);

        mfloat_t result[VEC4_SIZE];
        vec4_add(result, v0, v1);

        lua_newvector4(L, result[0], result[1], result[2], result[3]);
    }

    return 1;
}

static int modules_vector4_subtract(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);

    if (lua_isnumber(L, 2)) {
        mfloat_t f = luaL_checknumber(L, 2);

        lua_settop(L, 0);

        mfloat_t result[VEC4_SIZE];
        vec4_subtract_f(result, v0, f);

        lua_newvector4(L, result[0], result[1], result[2], result[3]);
    }
    else {
        mfloat_t* v1 = luaL_checkvector4(L, 2);

        lua_settop(L, 0);

        mfloat_t result[VEC4_SIZE];
        vec4_subtract(result, v0, v1);

        lua_newvector4(L, result[0], result[1], result[2], result[3]);
    }

    return 1;
}

static int modules_vector4_multiply(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);

    // Scalar multiplication
    if (lua_isnumber(L, 2)) {
        float f = luaL_checknumber(L, 2);

        lua_settop(L, 0);

        mfloat_t result[VEC4_SIZE];
        vec4_multiply_f(result, v0, f);

        lua_newvector4(L, result[0], result[1], result[2], result[3]);

        return 1;
    }

    // Component-wise multiplication
    mfloat_t* v1 = luaL_checkvector4(L, 2);

    lua_settop(L, 0);

    mfloat_t result[VEC4_SIZE];
    vec4_multiply(result, v0, v1);

    lua_newvector4(L, result[0], result[1], result[2], result[3]);

    return 1;
}

static int modules_vector4_divide(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);

    // Scalar division
    if (lua_isnumber(L, 2)) {
        float f = luaL_checknumber(L, 2);

        lua_settop(L, 0);

        mfloat_t result[VEC4_SIZE];
        vec4_divide_f(result, v0, f);

        lua_newvector4(L, result[0], result[1], result[2], result[3]);

        return 1;
    }

    // Component-wise division
    mfloat_t* v1 = luaL_checkvector4(L, 2);

    lua_settop(L, 0);

    mfloat_t result[VEC4_SIZE];
    vec4_divide(result, v0, v1);

    lua_newvector4(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Returns a vector made from snapping the components to given resolution.
 * @function snap
 * @tparam vector4 v0
 * @tparam number f Resolution of snap
 * @treturn vector4
 */
static int modules_vector4_snap(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);

    // Scalar snapping
    if (lua_isnumber(L, 2)) {
        float f = luaL_checknumber(L, 2);

        lua_settop(L, 0);

        mfloat_t result[VEC4_SIZE];
        vec4_snap_f(result, v0, f);

        lua_newvector4(L, result[0], result[1], result[2], result[3]);

        return 1;
    }

    // Component-wise snapping
    mfloat_t* v1 = luaL_checkvector4(L, 2);

    lua_settop(L, 0);

    mfloat_t result[VEC4_SIZE];
    vec4_snap(result, v0, v1);

    lua_newvector4(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Negates a vector.
 * @function negative
 * @tparam vector4 v0
 * @treturn vector4
 */
static int modules_vector4_negative(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);

    lua_settop(L, 0);

    mfloat_t result[VEC4_SIZE];
    vec4_negative(result, v0);

    lua_newvector4(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Returns a vector made from the absolute values of the components.
 * @function abs
 * @tparam vector4 v0
 * @treturn vector4
 */
static int modules_vector4_abs(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);

    lua_settop(L, 0);

    mfloat_t result[VEC4_SIZE];
    vec4_abs(result, v0);

    lua_newvector4(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Returns a vector made from the floor of the components.
 * @function floor
 * @tparam vector4 v0
 * @treturn vector4
 */
static int modules_vector4_floor(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);

    lua_settop(L, 0);

    mfloat_t result[VEC4_SIZE];
    vec4_floor(result, v0);

    lua_newvector4(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Returns a vector made from the ceil of the components.
 * @function ceil
 * @tparam vector4 v0
 * @treturn vector4
 */
static int modules_vector4_ceil(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);

    lua_settop(L, 0);

    mfloat_t result[VEC4_SIZE];
    vec4_ceil(result, v0);

    lua_newvector4(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Returns a vector made from rounding the components.
 * @function round
 * @tparam vector4 v0
 * @treturn vector4
 */
static int modules_vector4_round(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);

    lua_settop(L, 0);

    mfloat_t result[VEC4_SIZE];
    vec4_round(result, v0);

    lua_newvector4(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Returns a vector that is the component-wise max of v0 and v1.
 * @function max
 * @tparam vector4 v0
 * @tparam vector4 v1
 * @treturn vector4
 */
static int modules_vector4_max(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);
    mfloat_t* v1 = luaL_checkvector4(L, 2);

    lua_settop(L, 0);

    mfloat_t result[VEC4_SIZE];
    vec4_max(result, v0, v1);

    lua_newvector4(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Returns a vector that is the component-wise min of v0 and v1.
 * @function min
 * @tparam vector4 v0
 * @tparam vector4 v1
 * @treturn vector4
 */
static int modules_vector4_min(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);
    mfloat_t* v1 = luaL_checkvector4(L, 2);

    lua_settop(L, 0);

    mfloat_t result[VEC4_SIZE];
    vec4_min(result, v0, v1);

    lua_newvector4(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Returns a vector that is a component-wise clamp of v0 such that min < v0 < max.
 * @function clamp
 * @tparam vector4 v0 Vector to clamp
 * @tparam vector4 min Min vector
 * @tparam vector4 max Max vector
 * @treturn vector4
 */
static int modules_vector4_clamp(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);
    mfloat_t* v1 = luaL_checkvector4(L, 2);
    mfloat_t* v2 = luaL_checkvector4(L, 3);

    lua_settop(L, 0);

    mfloat_t result[VEC4_SIZE];
    vec4_clamp(result, v0, v1, v2);

    lua_newvector4(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Returns a vector in the direction of v0 with magnitude 1.
 * @function normalize
 * @tparam vector4 v0
 * @treturn vector4
 */
static int modules_vector4_normalize(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);

    lua_settop(L, 0);

    mfloat_t result[VEC4_SIZE];
    vec4_normalize(result, v0);

    lua_newvector4(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Linearly interpolate between v0 and v1.
 * @function lerp
 * @tparam vector4 v0
 * @tparam vector4 v1
 * @tparam number t Value used to interpolate between v0 and v1.
 * @treturn vector4
 */
static int modules_vector4_lerp(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);
    mfloat_t* v1 = luaL_checkvector4(L, 2);
    float f = luaL_checknumber(L, 3);

    lua_settop(L, 0);

    mfloat_t result[VEC4_SIZE];
    vec4_lerp(result, v0, v1, f);

    lua_newvector4(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Set vector components in place.
 * @function set
 * @tparam vector4 v0
 * @tparam vector4 v1
 */

/**
 * Set vector components in place.
 * @function set
 * @tparam vector4 v0
 * @tparam number x
 * @tparam number y
 * @tparam number z
 * @tparam number w
 */
static int modules_vector4_set(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector4(L, 1);

    if (lua_isvector4(L, 2)) {
        mfloat_t* v1 = luaL_checkvector4(L, 2);
        vec4_assign(v0, v1);

        return 0;
    }

    v0[0] = luaL_checknumber(L, 2);
    v0[1] = luaL_checknumber(L, 3);
    v0[2] = luaL_checknumber(L, 4);
    v0[3] = luaL_checknumber(L, 5);

    return 0;
}

static const struct luaL_Reg modules_vector4_functions[] = {
    {"new", modules_vector4_new},
    {"zero", modules_vector4_zero},
    {"one", modules_vector4_one},
    {"sign", modules_vector4_sign},
    {"add", modules_vector4_add},
    {"subtract", modules_vector4_subtract},
    {"multiply", modules_vector4_multiply},
    {"divide", modules_vector4_divide},
    {"snap", modules_vector4_snap},
    {"negative", modules_vector4_negative},
    {"abs", modules_vector4_abs},
    {"floor", modules_vector4_floor},
    {"ceil", modules_vector4_ceil},
    {"round", modules_vector4_round},
    {"max", modules_vector4_max},
    {"min", modules_vector4_min},
    {"clamp", modules_vector4_clamp},
    {"normalize", modules_vector4_normalize},
    {"lerp", modules_vector4_lerp},
    {"set", modules_vector4_set},
    {NULL, NULL}
};

static int modules_vector4_meta_index(lua_State* L) {
    mfloat_t* vector = luaL_checkvector4(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_settop(L, 0);

    if (strcmp(key, "x") == 0) {
        lua_pushnumber(L, vector[0]);
    }
    else if (strcmp(key, "y") == 0) {
        lua_pushnumber(L, vector[1]);
    }
    else if (strcmp(key, "z") == 0) {
        lua_pushnumber(L, vector[2]);
    }
    else if (strcmp(key, "w") == 0) {
        lua_pushnumber(L, vector[3]);
    }
    else {
        // Check module fields. This enables usage of the colon operator.
        luaL_requiref(L, "vector4", NULL, false);
        if (lua_type(L, -1) == LUA_TTABLE) {
            lua_getfield(L, -1, key);
        }
        else {
            lua_pushnil(L);
        }
    }

    return 1;
}

static int modules_vector4_meta_newindex(lua_State* L) {
    mfloat_t* vector = luaL_checkvector4(L, 1);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "x") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        vector[0] = value;
    }
    else if (strcmp(key, "y") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        vector[1] = value;
    }
    else if (strcmp(key, "z") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        vector[2] = value;
    }
    else if (strcmp(key, "w") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        vector[3] = value;
    }
    else {
        luaL_error(L, "attempt to index a vector4 value");
    }

    lua_settop(L, 0);

    return 0;
}

static const struct luaL_Reg modules_vector4_meta_functions[] = {
    {"__index", modules_vector4_meta_index},
    {"__newindex", modules_vector4_meta_newindex},
    {"__add", modules_vector4_add},
    {"__sub", modules_vector4_subtract},
    {"__mul", modules_vector4_multiply},
    {"__div", modules_vector4_divide},
    {"__unm", modules_vector4_negative},
    {"__eq", modules_vector4_equal},
    {NULL, NULL}
};

int luaopen_vector4(lua_State* L) {
    luaL_newlib(L, modules_vector4_functions);

    luaL_newmetatable(L, "vector4");
    luaL_setfuncs(L, modules_vector4_meta_functions, 0);

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, vector4_gc);
    lua_settable(L, -3);

    lua_pop(L, 1);

    luaL_newmetatable(L, "vector4_nogc");
    luaL_setfuncs(L, modules_vector4_meta_functions, 0);

    lua_pop(L, 1);

    return 1;
}
