/**
 * Module for working with vector3 data.
 * @module vector3
 */
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "vector3.h"

bool lua_isvector3(lua_State*L, int index) {
    void* p = luaL_testudata(L, index, "vector3");
    if (p == NULL) {
        p = luaL_testudata(L, index, "vector3_nogc");
    }

    return p != NULL;
}

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

/**
 * Vector3 class
 * @type vector3
 */

/**
 * Length of the vector.
 * @tfield number length
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
 * Functions
 * @section Functions
 */

/**
 * Returns a new vector3
 * @function new
 * @tparam ?number x
 * @tparam ?number y
 * @tparam ?number z
 * @treturn vector3
 */
static int modules_vector3_new(lua_State* L) {
    float x = (float)luaL_optnumber(L, 1, 0);
    float y = (float)luaL_optnumber(L, 2, 0);
    float z = (float)luaL_optnumber(L, 3, 0);

    lua_settop(L, 0);

    lua_newvector3(L, x, y, z);

    return 1;
}

static int modules_vector3_equal(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_settop(L, 0);

    bool result = vec3_is_equal(v0, v1);

    lua_pushboolean(L, result);

    return 1;
}

static int modules_vector3_zero(lua_State* L) {
    lua_newvector3(L, 0, 0, 0);
    mfloat_t* vector = luaL_checkvector3(L, 1);
    vec3_zero(vector);

    return 1;
}

static int modules_vector3_one(lua_State* L) {
    lua_newvector3(L, 0, 0, 0);
    mfloat_t* vector = luaL_checkvector3(L, 1);
    vec3_one(vector);

    return 1;
}

/**
 * Returns a vector made from the sign of it's components.
 * @function sign
 * @tparam vector3 v0
 * @treturn vector3
 */
static int modules_vector3_sign(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t result[VEC3_SIZE];
    vec3_sign(result, v0);
    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

static int modules_vector3_add(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    if (lua_isnumber(L, 2)) {
        mfloat_t f = luaL_checknumber(L, 2);

        lua_settop(L, 0);

        mfloat_t result[VEC3_SIZE];
        vec3_add_f(result, v0, f);

        lua_newvector3(L, result[0], result[1], result[2]);
    }
    else {
        mfloat_t* v1 = luaL_checkvector3(L, 2);

        lua_settop(L, 0);

        mfloat_t result[VEC3_SIZE];
        vec3_add(result, v0, v1);

        lua_newvector3(L, result[0], result[1], result[2]);
    }

    return 1;
}

static int modules_vector3_subtract(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    if (lua_isnumber(L, 2)) {
        mfloat_t f = luaL_checknumber(L, 2);

        lua_settop(L, 0);

        mfloat_t result[VEC3_SIZE];
        vec3_subtract_f(result, v0, f);

        lua_newvector3(L, result[0], result[1], result[2]);
    }
    else {
        mfloat_t* v1 = luaL_checkvector3(L, 2);

        lua_settop(L, 0);

        mfloat_t result[VEC3_SIZE];
        vec3_subtract(result, v0, v1);

        lua_newvector3(L, result[0], result[1], result[2]);
    }

    return 1;
}

static int modules_vector3_multiply(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    // Scalar multiplication
    if (lua_isnumber(L, 2)) {
        float f = luaL_checknumber(L, 2);

        lua_settop(L, 0);

        mfloat_t result[VEC3_SIZE];
        vec3_multiply_f(result, v0, f);

        lua_newvector3(L, result[0], result[1], result[2]);

        return 1;
    }

    // Component-wise multiplication
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_settop(L, 0);

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

static int modules_vector3_divide(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    // Scalar division
    if (lua_isnumber(L, 2)) {
        float f = luaL_checknumber(L, 2);

        lua_settop(L, 0);

        mfloat_t result[VEC3_SIZE];
        vec3_divide_f(result, v0, f);

        lua_newvector3(L, result[0], result[1], result[2]);

        return 1;
    }

    // Component-wise division
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_divide(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector made from snapping the components to given resolution.
 * @function snap
 * @tparam vector3 v0
 * @tparam number f Resolution of snap
 * @treturn vector3
 */
static int modules_vector3_snap(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    // Scalar snapping
    if (lua_isnumber(L, 2)) {
        float f = luaL_checknumber(L, 2);

        lua_settop(L, 0);

        mfloat_t result[VEC3_SIZE];
        vec3_snap_f(result, v0, f);

        lua_newvector3(L, result[0], result[1], result[2]);

        return 1;
    }

    // Component-wise snapping
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_snap(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Negates a vector.
 * @function negative
 * @tparam vector3 v0
 * @treturn vector3
 */
static int modules_vector3_negative(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_negative(result, v0);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector made from the absolute values of the components.
 * @function abs
 * @tparam vector3 v0
 * @treturn vector3
 */
static int modules_vector3_abs(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_abs(result, v0);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector made from the floor of the components.
 * @function floor
 * @tparam vector3 v0
 * @treturn vector3
 */
static int modules_vector3_floor(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_floor(result, v0);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector made from the ceil of the components.
 * @function ceil
 * @tparam vector3 v0
 * @treturn vector3
 */
static int modules_vector3_ceil(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_ceil(result, v0);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector made from rounding the components.
 * @function round
 * @tparam vector3 v0
 * @treturn vector3
 */
static int modules_vector3_round(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_round(result, v0);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector that is the component-wise max of v0 and v1.
 * @function max
 * @tparam vector3 v0
 * @tparam vector3 v1
 * @treturn vector3
 */
static int modules_vector3_max(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_max(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector that is the component-wise min of v0 and v1.
 * @function min
 * @tparam vector3 v0
 * @tparam vector3 v1
 * @treturn vector3
 */
static int modules_vector3_min(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_min(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector that is a component-wise clamp of v0 such that min < v0 < max.
 * @function clamp
 * @tparam vector3 v0 Vector to clamp
 * @tparam vector3 min Min vector
 * @tparam vector3 max Max vector
 * @treturn vector3
 */
static int modules_vector3_clamp(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);
    mfloat_t* v2 = luaL_checkvector3(L, 3);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_clamp(result, v0, v1, v2);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Cross product of two vectors.
 * @function cross
 * @tparam vector3 v0
 * @tparam vector3 v1
 * @treturn vector3
 */
static int modules_vector3_cross(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_cross(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Returns a vector in the direction of v0 with magnitude 1.
 * @function normalize
 * @tparam vector3 v0
 * @treturn vector3
 */
static int modules_vector3_normalize(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_normalize(result, v0);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Dot product of two vectors.
 * @function dot
 * @tparam vector3 v0
 * @tparam vector3 v1
 * @treturn number float
 */
static int modules_vector3_dot(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_settop(L, 0);

    float result = vec3_dot(v0, v1);

    lua_pushnumber(L, result);

    return 1;
}

/**
 * Project v0 onto v1.
 * @function project
 * @tparam vector3 v0
 * @tparam vector3 v1
 * @treturn vector3
 */
static int modules_vector3_project(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_project(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

static int modules_vector3_slide(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_slide(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Reflect v0 off of plane given by normal.
 * @function reflect
 * @tparam vector3 v0
 * @tparam vector3 normal Plane normal
 * @treturn vector3
 */
static int modules_vector3_reflect(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_reflect(result, v0, v1);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Rotate v0 around ra clockwise by angle in radians.
 * @function rotate
 * @tparam vector3 v0
 * @tparam vector3 ra Vector to rotate around
 * @tparam number radians Angle to rotate in radians
 * @treturn vector3
 */
static int modules_vector3_rotate(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* ra = luaL_checkvector3(L, 2);
    float f = luaL_checknumber(L, 3);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_rotate(result, v0, ra, f);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Linearly interpolate between v0 and v1.
 * @function lerp
 * @tparam vector3 v0
 * @tparam vector3 v1
 * @tparam number t Value used to interpolate between v0 and v1.
 * @treturn vector3
 */
static int modules_vector3_lerp(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);
    float f = luaL_checknumber(L, 3);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_lerp(result, v0, v1, f);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

// TODO: Doc this
static int modules_vector3_bezier3(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);
    mfloat_t* v2 = luaL_checkvector3(L, 3);
    float f = luaL_checknumber(L, 4);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_bezier3(result, v0, v1, v2, f);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

// TODO: Doc this
static int modules_vector3_bezier4(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);
    mfloat_t* v2 = luaL_checkvector3(L, 3);
    mfloat_t* v3 = luaL_checkvector3(L, 4);
    float f = luaL_checknumber(L, 5);

    lua_settop(L, 0);

    mfloat_t result[VEC3_SIZE];
    vec3_bezier4(result, v0, v1, v2, v3, f);

    lua_newvector3(L, result[0], result[1], result[2]);

    return 1;
}

/**
 * Get vector magnitude.
 * @function length
 * @tparam vector3 v0
 * @treturn number float
 */
static int modules_vector3_length(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    lua_settop(L, 0);

    float result = vec3_length(v0);

    lua_pushnumber(L, result);

    return 1;
}

/**
 * Get vector squared magnitude.
 * @function length_squared
 * @tparam vector3 v0
 * @treturn number float
 */
static int modules_vector3_length_squared(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    lua_settop(L, 0);

    float result = vec3_length_squared(v0);

    lua_pushnumber(L, result);

    return 1;
}

/**
 * Get distance between v0 and v1.
 * @function distance
 * @tparam vector3 v0
 * @tparam vector3 v1
 * @treturn number float
 */
static int modules_vector3_distance(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_settop(L, 0);

    float result = vec3_distance(v0, v1);

    lua_pushnumber(L, result);

    return 1;
}

/**
 * Get squared distance between v0 and v1.
 * @function distance_squared
 * @tparam vector3 v0
 * @tparam vector3 v1
 * @treturn number float
 */
static int modules_vector3_distance_squared(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_settop(L, 0);

    float result = vec3_distance_squared(v0, v1);

    lua_pushnumber(L, result);

    return 1;
}

/**
 * Set vector components in place.
 * @function set
 * @tparam vector3 v0
 * @tparam vector3 v1
 */

/**
 * Set vector components in place.
 * @function set
 * @tparam vector3 v0
 * @tparam number x
 * @tparam number y
 * @tparam number z
 */
static int modules_vector3_set(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);

    if (lua_isvector3(L, 2)) {
        mfloat_t* v1 = luaL_checkvector3(L, 2);
        vec3_assign(v0, v1);

        return 0;
    }

    v0[0] = luaL_checknumber(L, 2);
    v0[1] = luaL_checknumber(L, 3);
    v0[2] = luaL_checknumber(L, 4);

    return 0;
}


static const struct luaL_Reg modules_vector3_functions[] = {
    {"new", modules_vector3_new},
    {"zero", modules_vector3_zero},
    {"one", modules_vector3_one},
    {"sign", modules_vector3_sign},
    {"add", modules_vector3_add},
    {"subtract", modules_vector3_subtract},
    {"multiply", modules_vector3_multiply},
    {"divide", modules_vector3_divide},
    {"snap", modules_vector3_snap},
    {"negative", modules_vector3_negative},
    {"abs", modules_vector3_abs},
    {"floor", modules_vector3_floor},
    {"ceil", modules_vector3_ceil},
    {"round", modules_vector3_round},
    {"max", modules_vector3_max},
    {"min", modules_vector3_min},
    {"clamp", modules_vector3_clamp},
    {"cross", modules_vector3_cross},
    {"normalize", modules_vector3_normalize},
    {"dot", modules_vector3_dot},
    {"project", modules_vector3_project},
    {"slide", modules_vector3_slide},
    {"reflect", modules_vector3_reflect},
    {"rotate", modules_vector3_rotate},
    {"lerp", modules_vector3_lerp},
    {"bezier3", modules_vector3_bezier3},
    {"bezier4", modules_vector3_bezier4},
    {"length", modules_vector3_length},
    {"length_squared", modules_vector3_length_squared},
    {"distance", modules_vector3_distance},
    {"distance_squared", modules_vector3_distance_squared},
    {"set", modules_vector3_set},
    {NULL, NULL}
};

static int modules_vector3_meta_index(lua_State* L) {
    mfloat_t* vector = luaL_checkvector3(L, 1);
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
    else if (strcmp(key, "length") == 0) {
        lua_pushnumber(L, vec3_length(vector));
    }
    else {
        // Check module fields. This enables usage of the colon operator.
        luaL_requiref(L, "vector3", NULL, false);
        if (lua_type(L, -1) == LUA_TTABLE) {
            lua_getfield(L, -1, key);
        }
        else {
            lua_pushnil(L);
        }
    }

    return 1;
}

static int modules_vector3_meta_newindex(lua_State* L) {
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
        vector[2] = value;
    }
    else {
        luaL_error(L, "attempt to index a vector3 value");
    }

    lua_settop(L, 0);

    return 0;
}

static const struct luaL_Reg modules_vector3_meta_functions[] = {
    {"__index", modules_vector3_meta_index},
    {"__newindex", modules_vector3_meta_newindex},
    {"__add", modules_vector3_add},
    {"__sub", modules_vector3_subtract},
    {"__mul", modules_vector3_multiply},
    {"__div", modules_vector3_divide},
    {"__unm", modules_vector3_negative},
    {"__eq", modules_vector3_equal},
    {NULL, NULL}
};

int luaopen_vector3(lua_State* L) {
    luaL_newlib(L, modules_vector3_functions);

    luaL_newmetatable(L, "vector3");
    luaL_setfuncs(L, modules_vector3_meta_functions, 0);

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, vector3_gc);
    lua_settable(L, -3);

    lua_pop(L, 1);

    luaL_newmetatable(L, "vector3_nogc");
    luaL_setfuncs(L, modules_vector3_meta_functions, 0);

    lua_pop(L, 1);

    return 1;
}
