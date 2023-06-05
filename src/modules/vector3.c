/**
 * Module for working with vector3 data.
 * @module vector3
 */
#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "vector3.h"

mfloat_t* luaL_checkvector3(lua_State* L, int index) {
    mfloat_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);

    handle = (mfloat_t**)luaL_testudata(L, index, "vector3_nogc");
    if (!handle) {
        handle = (mfloat_t**)luaL_checkudata(L, index, "vector3");
    }

    return *handle;
}

int lua_newvector3(lua_State* L, float x, float y, float z) {
    mfloat_t** handle = (mfloat_t**)lua_newuserdata(L, sizeof(mfloat_t*));
    mfloat_t* vector = (mfloat_t*)malloc(sizeof(mfloat_t) * VEC3_SIZE);
    vector[0] = x;
    vector[1] = y;
    vector[2] = z;
    *handle = vector;
    luaL_setmetatable(L, "vector3");

    return 1;
}

int lua_pushvector3(lua_State* L, mfloat_t* vector) {
    mfloat_t** handle = (mfloat_t**)lua_newuserdata(L, sizeof(mfloat_t*));
    *handle = vector;

    luaL_setmetatable(L, "vector3_nogc");

    return 1;
}

static int vector3_gc(lua_State* L) {
    mfloat_t** handle = lua_touserdata(L, 1);
    free(*handle);
    *handle = NULL;

    return 0;
}

static int vector3_new(lua_State* L) {
    float x = (float)luaL_optnumber(L, 1, 0);
    float y = (float)luaL_optnumber(L, 2, 0);
    float z = (float)luaL_optnumber(L, 3, 0);

    lua_pop(L, -1);

    lua_newvector3(L, x, y, z);

    return 1;
}

static int vector3_equal(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_pop(L, -1);

    bool result = vec3_is_equal(v0, v1);

    lua_pushboolean(L, result);

    return 1;
}

static int vector3_add(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_add(&result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

static int vector3_subtract(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_subtract(&result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

static int vector3_multiply(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    // Scalar multiplication
    if (lua_isnumber(L, 2)) {
        float f = luaL_checknumber(L, 2);

        lua_pop(L, -1);

        mfloat_t result[VEC3_SIZE];
        vec3_multiply_f(result, v0, f);

        lua_newvector3(L, result[0], result[1], result[2]);

        return 1;
    }

    // Component-wise multiplication
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_multiply(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

// TODO: Matrix math
// static int vector3_multiply_mat3(lua_State* L) {
//     mfloat_t* v0 = luaL_checkvector3(L, 1);

//     vec3_multiply_mat3(mfloat_t *result, mfloat_t *v0, mfloat_t *m0);

//     return 1;
// }

static int vector3_divide(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    // Scalar division
    if (lua_isnumber(L, 2)) {
        float f = luaL_checknumber(L, 2);

        lua_pop(L, -1);

        mfloat_t result[VEC3_SIZE];
        vec3_divide_f(result, v0, f);

        lua_newvector3(L, result[0], result[1], result[2]);

        return 1;
    }

    // Component-wise division
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_divide(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector made from snapping the components to given resolution.
 * @function snap
 * @param v0
 * @param f Resolution of snap
 * @return vector3
 */
static int vector3_snap(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    // Scalar snapping
    if (lua_isnumber(L, 2)) {
        float f = luaL_checknumber(L, 2);

        lua_pop(L, -1);

        mfloat_t result[VEC3_SIZE];
        vec3_snap_f(result, v0, f);

        lua_newvector3(L, result[0], result[1], result[2]);

        return 1;
    }

    // Component-wise snapping
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_snap(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Negates a vector.
 * @function negative
 * @param v0
 * @return vector3
 */
static int vector3_negative(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_negative(result, v0);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector made from the absolute values of the components.
 * @function abs
 * @param v0
 * @return vector3
 */
static int vector3_abs(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_abs(result, v0);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector made from the floor of the components.
 * @function floor
 * @param v0
 * @return vector3
 */
static int vector3_floor(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_floor(result, v0);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector made from the ceil of the components.
 * @function ceil
 * @param v0
 * @return vector3
 */
static int vector3_ceil(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_ceil(result, v0);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector made from rounding the components.
 * @function round
 * @param v0
 * @return vector3
 */
static int vector3_round(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_round(result, v0);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector that is the component-wise max of v0 and v1.
 * @function max
 * @param v0
 * @param v1
 * @return vector3
 */
static int vector3_max(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_max(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector that is the component-wise min of v0 and v1.
 * @function max
 * @param v0
 * @param v1
 * @return vector3
 */
static int vector3_min(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_min(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector that is a component-wise clamp of v0 such that min < v0 < max.
 * @function clamp
 * @param v0 Vector to clamp
 * @param min Min vector
 * @param max Max vector
 * @return vector3
 */
static int vector3_clamp(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);
    mfloat_t* v2 = luaL_checkvector3(L, 3);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_clamp(result, v0, v1, v2);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Cross product of two vectors.
 * @function cross
 * @param v0
 * @param v1
 * @return vector3
 */
static int vector3_cross(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_cross(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector in the direction of v0 with magnitude 1.
 * @function normalize
 * @param v0
 * @return vector3
 */
static int vector3_normalize(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_normalize(result, v0);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Dot product of two vectors.
 * @function dot
 * @param v0
 * @param v1
 * @return float
 */
static int vector3_dot(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_pop(L, -1);

    float result = vec3_dot(v0, v1);

    lua_pushnumber(L, result);

    return 1;
}

/**
 * Project v0 onto v1.
 * @function project
 * @param v0
 * @param v1
 * @return vector3
 */
static int vector3_project(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_project(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

static int vector3_slide(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_slide(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Reflect v0 off of plane given by normal.
 * @function reflect
 * @param v0
 * @param normal Plane normal
 * @return vector3
 */
static int vector3_reflect(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_reflect(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Rotate v0 around ra clockwise by angle in radians.
 * @function rotate
 * @param v0
 * @param ra Vector to rotate around
 * @param angle Angle to rotate in radians
 * @return vector3
 */
static int vector3_rotate(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* ra = luaL_checkvector3(L, 2);
    float f = luaL_checknumber(L, 3);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_rotate(result, v0, ra, f);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Linearly interpolate between v0 and v1.
 * @function lerp
 * @param v0
 * @param v1
 * @param t Value used to interpolate between v0 and v1.
 * @return vector3
 */
static int vector3_lerp(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);
    float f = luaL_checknumber(L, 3);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_lerp(result, v0, v1, f);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

// TODO: Doc this
static int vector3_bezier3(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);
    mfloat_t* v2 = luaL_checkvector3(L, 3);
    float f = luaL_checknumber(L, 4);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_bezier3(result, v0, v1, v2, f);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

// TODO: Doc this
static int vector3_bezier4(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);
    mfloat_t* v2 = luaL_checkvector3(L, 3);
    mfloat_t* v3 = luaL_checkvector3(L, 4);
    float f = luaL_checknumber(L, 5);

    lua_pop(L, -1);

    mfloat_t result[VEC3_SIZE];
    vec3_bezier4(result, v0, v1, v2, v3, f);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Get vector magnitude.
 * @function length
 * @param v0
 * @return float
 */
static int vector3_length(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    lua_pop(L, -1);

    float result = vec3_length(v0);

    lua_pushnumber(L, result);

    return 1;
}

/**
 * Get vector squared magnitude.
 * @function length_squared
 * @param v0
 * @return float
 */
static int vector3_length_squared(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    lua_pop(L, -1);

    float result = vec3_length_squared(v0);

    lua_pushnumber(L, result);

    return 1;
}

/**
 * Get distance between v0 and v1.
 * @function distance
 * @param v0
 * @param v1
 * @return float
 */
static int vector3_distance(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_pop(L, -1);

    float result = vec3_distance(v0, v1);

    lua_pushnumber(L, result);

    return 1;
}

/**
 * Get squared distance between v0 and v1.
 * @function distance_squared
 * @param v0
 * @param v1
 * @return float
 */
static int vector3_distance_squared(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_pop(L, -1);

    float result = vec3_distance_squared(v0, v1);

    lua_pushnumber(L, result);

    return 1;
}


static const struct luaL_Reg module_functions[] = {
    {"new", vector3_new},
    {"add", vector3_add},
    {"subtract", vector3_subtract},
    {"multiply", vector3_multiply},
    {"divide", vector3_divide},
    {"snap", vector3_snap},
    {"negative", vector3_negative},
    {"abs", vector3_abs},
    {"floor", vector3_floor},
    {"ceil", vector3_ceil},
    {"round", vector3_round},
    {"max", vector3_max},
    {"min", vector3_min},
    {"clamp", vector3_clamp},
    {"cross", vector3_cross},
    {"normalize", vector3_normalize},
    {"dot", vector3_dot},
    {"project", vector3_project},
    {"slide", vector3_slide},
    {"reflect", vector3_reflect},
    {"rotate", vector3_rotate},
    {"lerp", vector3_lerp},
    {"bezier3", vector3_bezier3},
    {"bezier4", vector3_bezier4},
    {"length", vector3_length},
    {"length_squared", vector3_length_squared},
    {"distance", vector3_distance},
    {"distance_squared", vector3_distance_squared},
    {NULL, NULL}
};

static int vector3_meta_index(lua_State* L) {
    mfloat_t* vector = luaL_checkvector3(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_pop(L, -1);

    if (strcmp(key, "x") == 0) {
        lua_pushnumber(L, vector[0]);
    }
    else if (strcmp(key, "y") == 0) {
        lua_pushnumber(L, vector[1]);
    }
    else if (strcmp(key, "z") == 0) {
        lua_pushnumber(L, vector[2]);
    }
    else if (strcmp(key, "length") == 0) {
        lua_pushnumber(L, vec3_length(vector));
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static int vector3_meta_newindex(lua_State* L) {
    mfloat_t* vector = luaL_checkvector3(L, 1);
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
        vector[1] = value;
    }
    else {
        luaL_error(L, "attempt to index a vector3 value");
    }

    lua_pop(L, -1);

    return 0;
}

static const struct luaL_Reg meta_functions[] = {
    {"__index", vector3_meta_index},
    {"__newindex", vector3_meta_newindex},
    {"__add", vector3_add},
    {"__sub", vector3_subtract},
    {"__mul", vector3_multiply},
    {"__div", vector3_divide},
    {"__unm", vector3_negative},
    {"__eq", vector3_equal},
    {NULL, NULL}
};

int luaopen_vector3(lua_State* L) {
    luaL_newlib(L, module_functions);

    luaL_newmetatable(L, "vector3");
    luaL_setfuncs(L, meta_functions, 0);

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, vector3_gc);
    lua_settable(L, -3);

    lua_pop(L, 1);

    luaL_newmetatable(L, "vector3_nogc");
    luaL_setfuncs(L, meta_functions, 0);

    lua_pop(L, 1);

    return 1;
}
