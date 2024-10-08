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

static int modules_quaternion_gc(lua_State* L) {
    mfloat_t** handle = lua_touserdata(L, 1);
    free(*handle);
    *handle = NULL;

    return 0;
}

/**
 * Quaternion class
 * @type quaternion
 */

/**
 * Conjugate of quaternion.
 * @function conjugate
 * @treturn quaternion
 */
static int modules_quaternion_conjugate(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_conjugate(result, q0);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Inverse of quaternion.
 * @function inverse
 * @treturn quaternion
 */
static int modules_quaternion_inverse(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_inverse(result, q0);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Get quaternion magnitude
 * @function length
 * @treturn number float
 */
static int modules_quaternion_length(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    lua_settop(L, 0);

    mfloat_t length = quat_length(q0);

    lua_pushnumber(L, length);

    return 1;
}

/**
 * Get quaternion squared magnitude
 * @function length_squared
 * @treturn number float
 */
static int modules_quaternion_length_squared(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    lua_settop(L, 0);

    mfloat_t length = quat_length_squared(q0);

    lua_pushnumber(L, length);

    return 1;
}

/**
 * X component of the quaternion
 * @tfield number x
 */

/**
 * Y component of the quaternion
 * @tfield number y
 */

/**
 * Z component of the quaternion
 * @tfield number z
 */

/**
 * W component of the quaternion
 * @tfield number w
 */

/**
 * Functions
 * @section Functions
 */

/**
 * Returns a new quaternion.
 * @function new
 * @tparam number x
 * @tparam number y
 * @tparam number z
 * @tparam number w
 * @treturn quaternion
 */
static int modules_quaternion_new(lua_State* L) {
    float x = (float)luaL_optnumber(L, 1, 0);
    float y = (float)luaL_optnumber(L, 2, 0);
    float z = (float)luaL_optnumber(L, 3, 0);
    float w = (float)luaL_optnumber(L, 4, 0);

    lua_settop(L, 0);

    lua_newquaternion(L, x, y, z, w);

    return 1;
}

/**
 * Returns a quaternion with all components set to zero.
 * @treturn quaternion
 */
static int modules_quaternion_zero(lua_State* L) {
    lua_newquaternion(L, 0, 0, 0, 0);
    mfloat_t* result = luaL_checkquaternion(L, 1);
    quat_zero(result);

    return 1;
}

/**
 * Returns a quaternion representing no rotation.
 * @treturn quaternion
 */
static int modules_quaternion_identity(lua_State* L) {
    lua_newquaternion(L, 0, 0, 0, 0);
    mfloat_t* result = luaL_checkquaternion(L, 1);
    quat_null(result);

    return 1;
}

static int modules_quaternion_multiply(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    // Scalar multiplication
    if (lua_isnumber(L, 2)) {
        float f = luaL_checknumber(L, 2);

        lua_settop(L, 0);

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

static int modules_quaternion_divide(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    // Scalar multiplication
    if (lua_isnumber(L, 2)) {
        float f = luaL_checknumber(L, 2);

        lua_settop(L, 0);

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

static int modules_quaternion_negative(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_negative(result, q0);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

static int modules_quaternion_equal(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);
    mfloat_t* q1 = luaL_checkquaternion(L, 2);

    lua_settop(L, 0);

    bool result = quat_is_equal(q0, q1);

    lua_pushboolean(L, result);

    return 1;
}

/**
 * Creates quaternion with same rotation, but magnitude of 1.
 * @function normalize
 * @tparam quaternion q0
 * @treturn quaternion
 */
static int modules_quaternion_normalize(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_normalize(result, q0);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Dot product of two quaternions.
 * @function normalize
 * @tparam quaternion q0
 * @tparam quaternion q1
 * @treturn number float
 */
static int modules_quaternion_dot(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);
    mfloat_t* q1 = luaL_checkquaternion(L, 2);

    lua_settop(L, 0);

    mfloat_t result = quat_dot(q0, q1);

    lua_pushnumber(L, result);

    return 1;
}

static int modules_quaternion_power(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);
    mfloat_t exponent = luaL_checknumber(L, 2);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_power(result, q0, exponent);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Creates a rotation from a unit vector and angle to rotate around the vector.
 * @function from_axis_angle
 * @tparam vector3.vector3 v0
 * @tparam number radians
 * @treturn quaternion
 */
static int modules_quaternion_from_axis_angle(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t angle = luaL_checknumber(L, 2);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_from_axis_angle(result, v0, angle);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Creates a rotation from v0 to v1.
 * @function from_vector3
 * @tparam vector3.vector3 v0
 * @tparam vector3.vector3 v1
 * @treturn quaternion
 */
static int modules_quaternion_from_vector3(lua_State* L) {
    mfloat_t* v0 = luaL_checkvector3(L, 1);
    mfloat_t* v1 = luaL_checkvector3(L, 2);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_from_vec3(result, v0, v1);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Create a rotation from a rotation matrix.
 * @function from_matrix4
 * @tparam matrix4.matrix4 m0
 * @treturn quaternion
 */
static int modules_quaternion_from_matrix4(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix4(L, 1);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_from_mat4(result, m0);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Create a rotation from Euler angles.
 * @function from_euler
 * @tparam vector3.vector3 v0
 * @treturn quaternion
 */

/**
 * Create a rotation from Euler angles.
 * @function from_euler
 * @tparam number x
 * @tparam number y
 * @tparam number z
 * @treturn quaternion
 */
static int modules_quaternion_from_euler(lua_State* L) {
    int arg_count = lua_gettop(L);

    mfloat_t x_angle = 0.0f;
    mfloat_t y_angle = 0.0f;
    mfloat_t z_angle = 0.0f;

    if (arg_count == 1) {
        mfloat_t* v0 = luaL_checkvector3(L, 1);
        x_angle = v0[0];
        y_angle = v0[1];
        z_angle = v0[2];
    }
    else {
        x_angle = luaL_checknumber(L, 1);
        y_angle = luaL_checknumber(L, 2);
        z_angle = luaL_checknumber(L, 3);
    }

    lua_settop(L, 0);

    mfloat_t hx = x_angle * MFLOAT_C(0.5);
    mfloat_t sx = MSIN(hx);
    mfloat_t cx = MCOS(hx);
    mfloat_t hy = y_angle * MFLOAT_C(0.5);
    mfloat_t sy = MSIN(hy);
    mfloat_t cy = MCOS(hy);
    mfloat_t hz = z_angle * MFLOAT_C(0.5);
    mfloat_t sz = MSIN(hz);
    mfloat_t cz = MCOS(hz);

    mfloat_t cy_sx = cy * sx;
    mfloat_t sy_cx = sy * cx;
    mfloat_t cy_cx = cy * cx;
    mfloat_t sy_sx = sy * sx;

    mfloat_t result[QUAT_SIZE];

    result[0] = (cy_sx * cz) + (sy_cx * sz);
    result[1] = (sy_cx * cz) - (cy_sx * sz);
    result[2] = (cy_cx * sz) - (sy_sx * cz);
    result[3] = (cy_cx * cz) + (sy_sx * sz);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Return Euler angle representation of rotation.
 * @function to_euler
 * @tparam quaternion q0
 * @treturn vector3.vector3
 */
static int modules_quaternion_to_euler(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    lua_settop(L, 0);

    mfloat_t pole = q0[1] * q0[0] + q0[2] * q0[3];

    if (pole > 0.499f) {
        pole = 1.0f;
    }
    else if (pole < -0.499f) {
        pole = -1.0f;
    }
    else {
        pole = 0.0f;
    }

    mfloat_t x_angle = 0.0f;
    mfloat_t y_angle = 0.0f;
    mfloat_t z_angle = 0.0f;

    if (pole == 0) {
        x_angle = MASIN(clampf(2.0f * (q0[3] * q0[0] - q0[2] * q0[1]), -1.0f, 1.0f));
        y_angle = MATAN2(2.0f * (q0[1] * q0[3] + q0[0] * q0[2]), 1.0f - 2.0f * (q0[1] * q0[1] + q0[0] * q0[0]));
        z_angle = MATAN2(2.0f * (q0[3] * q0[2] + q0[1] * q0[0]), 1.0f - 2.0f * (q0[0] * q0[0] + q0[2] * q0[2]));
    }
    else {
        x_angle = pole * MPI_2;
        y_angle = 0.0f;
        z_angle = pole * 2.0f * MATAN2(q0[1], q0[3]);
    }

    lua_newvector3(L, x_angle, y_angle, z_angle);

    return 1;
}

/**
 * Linearly interpolate between q0 and q1.
 * @function lerp
 * @tparam quaternion q0
 * @tparam quaternion q1
 * @tparam number t Value used to interpolate between q0 and q1.
 * @treturn quaternion
 */
static int modules_quaternion_lerp(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);
    mfloat_t* q1 = luaL_checkquaternion(L, 2);
    mfloat_t f = luaL_checknumber(L, 3);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_lerp(result, q0, q1, f);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Linearly interpolate between q0 and q1, using spherical linear interpolation.
 * @function slerp
 * @tparam quaternion q0
 * @tparam quaternion q1
 * @tparam number t Value used to interpolate between q0 and q1.
 * @treturn quaternion
 */
static int modules_quaternion_slerp(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);
    mfloat_t* q1 = luaL_checkquaternion(L, 2);
    mfloat_t f = luaL_checknumber(L, 3);

    lua_settop(L, 0);

    mfloat_t result[QUAT_SIZE];
    quat_slerp(result, q0, q1, f);

    lua_newquaternion(L, result[0], result[1], result[2], result[3]);

    return 1;
}

/**
 * Get angle between two rotations
 * @function angle
 * @tparam quaternion q0
 * @tparam quaternion q1
 * @treturn number float
 */
static int modules_quaternion_angle(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);
    mfloat_t* q1 = luaL_checkquaternion(L, 2);

    lua_settop(L, 0);

    mfloat_t angle = quat_angle(q0, q1);

    lua_pushnumber(L, angle);

    return 1;
}

static const struct luaL_Reg modules_quaternion_functions[] = {
    // Class functions
    {"conjugate", modules_quaternion_conjugate},
    {"inverse", modules_quaternion_inverse},
    {"normalize", modules_quaternion_normalize},
    {"length", modules_quaternion_length},
    {"length_squared", modules_quaternion_length_squared},
    // Module functions
    {"new", modules_quaternion_new},
    {"zero", modules_quaternion_zero},
    {"identity", modules_quaternion_identity},
    {"dot", modules_quaternion_dot},
    {"from_axis_angle", modules_quaternion_from_axis_angle},
    {"from_vector3", modules_quaternion_from_vector3},
    {"from_matrix4", modules_quaternion_from_matrix4},
    {"from_euler", modules_quaternion_from_euler},
    {"to_euler", modules_quaternion_to_euler},
    {"lerp", modules_quaternion_lerp},
    {"slerp", modules_quaternion_slerp},
    {"angle", modules_quaternion_angle},
    {NULL, NULL}
};

static int modules_quaternion_meta_index(lua_State* L) {
    mfloat_t* quaternion = luaL_checkquaternion(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_settop(L, 0);

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

static int modules_quaternion_meta_newindex(lua_State* L) {
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

    lua_settop(L, 0);

    return 0;
}

static const struct luaL_Reg modules_quaternion_meta_functions[] = {
    {"__index", modules_quaternion_meta_index},
    {"__newindex", modules_quaternion_meta_newindex},
    {"__mul", modules_quaternion_multiply},
    {"__div", modules_quaternion_divide},
    {"__unm", modules_quaternion_negative},
    {"__eq", modules_quaternion_equal},
    {"__pow", modules_quaternion_power},
    {NULL, NULL}
};

int luaopen_quaternion(lua_State* L) {
    luaL_newlib(L, modules_quaternion_functions);

    luaL_newmetatable(L, "quaternion");
    luaL_setfuncs(L, modules_quaternion_meta_functions, 0);

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, modules_quaternion_gc);
    lua_settable(L, -3);

    lua_pop(L, 1);

    luaL_newmetatable(L, "quaternion_nogc");
    luaL_setfuncs(L, modules_quaternion_meta_functions, 0);

    lua_pop(L, 1);

    return 1;
}
