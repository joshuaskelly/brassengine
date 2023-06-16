/**
 * Module for working with quaternion data.
 * @module quaternion
 */
#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "matrix4.h"
#include "quaternion.h"
#include "vector3.h"

mfloat_t* luaL_checkquaternion(lua_State* L, int index) {
    mfloat_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);

    handle = (mfloat_t**)luaL_testudata(L, index, "quaternion_nogc");
    if (!handle) {
        handle = (mfloat_t**)luaL_checkudata(L, index, "quaternion");
    }

    return *handle;
}

int lua_newquaternion(lua_State* L, float x, float y, float z, float w) {
    mfloat_t** handle = (mfloat_t**)lua_newuserdata(L, sizeof(mfloat_t*));
    mfloat_t* q0 = (mfloat_t*)malloc(sizeof(mfloat_t) * QUAT_SIZE);

    q0[0] = x;
    q0[1] = y;
    q0[2] = z;
    q0[3] = w;

    *handle = q0;

    luaL_setmetatable(L, "quaternion");

    return 1;
}

int lua_pushquaternion(lua_State* L, mfloat_t* quaternion) {
    mfloat_t** handle = (mfloat_t**)lua_newuserdata(L, sizeof(mfloat_t*));
    *handle = quaternion;

    luaL_setmetatable(L, "quaternion_nogc");

    return 1;
}

static int quaternion_gc(lua_State* L) {
    mfloat_t** handle = lua_touserdata(L, 1);
    free(*handle);
    *handle = NULL;

    return 0;
}

/**
 * Resturns a new quaternion.
 * @function new
 * @param x
 * @param y
 * @param z
 * @param w
 * @return @{quaternion}
 */
static int quaternion_new(lua_State* L) {
    float x = (float)luaL_optnumber(L, 1, 0);
    float y = (float)luaL_optnumber(L, 2, 0);
    float z = (float)luaL_optnumber(L, 3, 0);
    float w = (float)luaL_optnumber(L, 4, 0);

    lua_settop(L, 0);

    lua_newquaternion(L, x, y, z, w);

    return 1;
}

static int quaternion_zero(lua_State* L) {
    lua_newquaternion(L, 0, 0, 0, 0);
    mfloat_t* result = luaL_checkquaternion(L, 1);
    quat_zero(result);

    return 1;
}

static int quaternion_multiply(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    // Scalar multiplication
    if (lua_isnumber(L, 2)) {
        float f = luaL_checknumber(L, 2);

        lua_pop(L, -1);

        mfloat_t result[QUAT_SIZE];
        quat_multiply_f(result, q0, f);

        lua_newquaternion(L, result[0], result[1], result[2], result[3]);

        return 1;
    }

    // Component-wise multiplication
    mfloat_t* q1 = luaL_checkquaternion(L, 2);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_multiply(result, q0, q1);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

static int quaternion_divide(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    // Scalar multiplication
    if (lua_isnumber(L, 2)) {
        float f = luaL_checknumber(L, 2);

        lua_pop(L, -1);

        mfloat_t result[QUAT_SIZE];
        quat_divide_f(result, q0, f);

        lua_newquaternion(L, result[0], result[1], result[2], result[3]);

        return 1;
    }

    // Component-wise multiplication
    mfloat_t* q1 = luaL_checkquaternion(L, 2);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_divide(result, q0, q1);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

static int quaternion_negative(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_negative(result, q0);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

static int quaternion_equal(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);
    mfloat_t* q1 = luaL_checkquaternion(L, 2);

    lua_settop(L, 0);

    bool result = quat_is_equal(q0, q1);

    lua_pushboolean(L, result);

    return 1;
}

static int quaternion_conjugate(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_conjugate(result, q0);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

static int quaternion_inverse(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_inverse(result, q0);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

static int quaternion_normalize(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_normalize(result, q0);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

static int quaternion_dot(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);
    mfloat_t* q1 = luaL_checkquaternion(L, 2);

    lua_settop(L, 0);

    mfloat_t result = quat_dot(q0, q1);

    lua_pushnumber(L, result);

    return 1;
}

static int quaternion_power(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);
    mfloat_t exponent = luaL_checknumber(L, 2);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_power(result, q0, exponent);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

static int quaternion_from_axis_angle(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t angle = luaL_checknumber(L, 2);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_from_axis_angle(result, v0, angle);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

static int quaternion_from_vector3(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_from_vec3(result, v0, v1);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

static int quaternion_from_matrix4(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix4(L, 1);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_from_mat4(result, m0);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

static int quaternion_lerp(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);
    mfloat_t* q1 = luaL_checkquaternion(L, 2);
    mfloat_t f = luaL_checknumber(L, 3);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_lerp(result, q0, q1, f);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

static int quaternion_slerp(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);
    mfloat_t* q1 = luaL_checkquaternion(L, 2);
    mfloat_t f = luaL_checknumber(L, 3);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_slerp(result, q0, q1, f);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

static int quaternion_length(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    lua_settop(L, 0);

    mfloat_t length = quat_length(q0);

    lua_pushnumber(L, length);

    return 1;
}

static int quaternion_length_squared(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    lua_settop(L, 0);

    mfloat_t length = quat_length_squared(q0);

    lua_pushnumber(L, length);

    return 1;
}

static int quaternion_angle(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);
    mfloat_t* q1 = luaL_checkquaternion(L, 2);

    lua_settop(L, 0);

    mfloat_t angle = quat_angle(q0, q1);

    lua_pushnumber(L, angle);

    return 1;
}

static const struct luaL_Reg module_functions[] = {
    {"new", quaternion_new},
    {"zero", quaternion_zero},
    {"conjugate", quaternion_conjugate},
    {"inverse", quaternion_inverse},
    {"normalize", quaternion_normalize},
    {"dot", quaternion_dot},
    {"from_axis_angle", quaternion_from_axis_angle},
    {"from_vector3", quaternion_from_vector3},
    {"from_matrix4", quaternion_from_matrix4},
    {"lerp", quaternion_lerp},
    {"slerp", quaternion_slerp},
    {"length", quaternion_length},
    {"length_squared", quaternion_length_squared},
    {"angle", quaternion_angle},
    {NULL, NULL}
};

static int quaternion_meta_index(lua_State* L) {
    mfloat_t* quaternion = luaL_checkquaternion(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_pop(L, -1);

    if (strcmp(key, "x") == 0) {
        lua_pushnumber(L, quaternion[0]);
    }
    else if (strcmp(key, "y") == 0) {
        lua_pushnumber(L, quaternion[1]);
    }
    else if (strcmp(key, "z") == 0) {
        lua_pushnumber(L, quaternion[2]);
    }
    else if (strcmp(key, "w") == 0) {
        lua_pushnumber(L, quaternion[3]);
    }
    else {
        // Check module fields. This enables usage of the colon operator.
        luaL_requiref(L, "quaternion", NULL, false);
        if (lua_type(L, -1) == LUA_TTABLE) {
            lua_getfield(L, -1, key);
        }
        else {
            lua_pushnil(L);
        }
    }

    return 1;
}

static int quaternion_meta_newindex(lua_State* L) {
    mfloat_t* vector = luaL_checkquaternion(L, 1);
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
        luaL_error(L, "attempt to index a quaternion value");
    }

    lua_pop(L, -1);

    return 0;
}

static const struct luaL_Reg meta_functions[] = {
    {"__index", quaternion_meta_index},
    {"__newindex", quaternion_meta_newindex},
    {"__mul", quaternion_multiply},
    {"__div", quaternion_divide},
    {"__unm", quaternion_negative},
    {"__eq", quaternion_equal},
    {"__pow", quaternion_power},
    {NULL, NULL}
};

int luaopen_quaternion(lua_State* L) {
    luaL_newlib(L, module_functions);

    luaL_newmetatable(L, "quaternion");
    luaL_setfuncs(L, meta_functions, 0);

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, quaternion_gc);
    lua_settable(L, -3);

    lua_pop(L, 1);

    luaL_newmetatable(L, "quaternion_nogc");
    luaL_setfuncs(L, meta_functions, 0);

    lua_pop(L, 1);

    return 1;
}
