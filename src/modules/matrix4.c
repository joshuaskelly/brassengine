/**
 * Module for working with matrix4 data.
 * @module matrix4
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
#include "vector4.h"

bool lua_ismatrix4(lua_State*L, int index) {
    void* p = luaL_testudata(L, index, "matrix4");
    if (p == NULL) {
        p = luaL_testudata(L, index, "matrix4_nogc");
    }

    return p != NULL;
}

mfloat_t* luaL_checkmatrix4(lua_State* L, int index) {
    mfloat_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);

    handle = (mfloat_t**)luaL_testudata(L, index, "matrix4_nogc");
    if (!handle) {
        handle = (mfloat_t**)luaL_checkudata(L, index, "matrix4");
    }

    return *handle;
}

int lua_newmatrix4(lua_State* L, float m11, float m21, float m31, float m41, float m12, float m22, float m32, float m42, float m13, float m23, float m33, float m43, float m14, float m24, float m34, float m44) {
    mfloat_t** handle = (mfloat_t**)lua_newuserdata(L, sizeof(mfloat_t*));
    mfloat_t* m0 = (mfloat_t*)malloc(sizeof(mfloat_t) * MAT4_SIZE);
    m0[0] = m11;
    m0[1] = m21;
    m0[2] = m31;
    m0[3] = m41;
    m0[4] = m12;
    m0[5] = m22;
    m0[6] = m32;
    m0[7] = m42;
    m0[8] = m13;
    m0[9] = m23;
    m0[10] = m33;
    m0[11] = m43;
    m0[12] = m14;
    m0[13] = m24;
    m0[14] = m34;
    m0[15] = m44;

    *handle = m0;

    luaL_setmetatable(L, "matrix4");

    return 1;
}

int lua_newmatrix4_from_matrix(lua_State* L, mfloat_t* m0) {
    mfloat_t** handle = (mfloat_t**)lua_newuserdata(L, sizeof(mfloat_t*));
    mfloat_t* m1 = (mfloat_t*)malloc(sizeof(mfloat_t) * MAT4_SIZE);
    m1[0] = m0[0];
    m1[1] = m0[1];
    m1[2] = m0[2];
    m1[3] = m0[3];
    m1[4] = m0[4];
    m1[5] = m0[5];
    m1[6] = m0[6];
    m1[7] = m0[7];
    m1[8] = m0[8];
    m1[9] = m0[9];
    m1[10] = m0[10];
    m1[11] = m0[11];
    m1[12] = m0[12];
    m1[13] = m0[13];
    m1[14] = m0[14];
    m1[15] = m0[15];

    *handle = m1;

    luaL_setmetatable(L, "matrix4");

    return 1;
}

int lua_pushmatrix4(lua_State* L, mfloat_t* matrix) {
    mfloat_t** handle = (mfloat_t**)lua_newuserdata(L, sizeof(mfloat_t*));
    *handle = matrix;

    luaL_setmetatable(L, "matrix4_nogc");

    return 1;
}

static int modules_matrix4_gc(lua_State* L) {
    mfloat_t** handle = lua_touserdata(L, 1);
    free(*handle);
    *handle = NULL;

    return 0;
}

/**
 * Matrix4 class
 * @type matrix4
 */

/** @tfield number m11 */
/** @tfield number m21 */
/** @tfield number m31 */
/** @tfield number m41 */
/** @tfield number m12 */
/** @tfield number m22 */
/** @tfield number m32 */
/** @tfield number m42 */
/** @tfield number m13 */
/** @tfield number m23 */
/** @tfield number m33 */
/** @tfield number m43 */
/** @tfield number m14 */
/** @tfield number m24 */
/** @tfield number m34 */
/** @tfield number m44 */

/**
 * Determinant of matrix.
 * @function determinant
 * @treturn number float
 */
static int modules_matrix4_determinant(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix4(L, 1);
    float determinant = mat4_determinant(m0);

    lua_settop(L, 0);

    lua_pushnumber(L, determinant);

    return 1;
}

/**
 * Negates a matrix.
 * @function negative
 * @treturn matrix4
 */
static int modules_matrix4_negative(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix4(L, 1);

    lua_settop(L, 0);

    mfloat_t result[MAT4_SIZE];
    mat4_negative(result, m0);

    lua_newmatrix4_from_matrix(L, result);

    return 1;
}

/**
 * Transpose of matrix.
 * @function transpose
 * @treturn matrix4
 */
static int modules_matrix4_transpose(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix4(L, 1);

    lua_settop(L, 0);

    mfloat_t result[MAT4_SIZE];
    mat4_transpose(result, m0);

    lua_newmatrix4_from_matrix(L, result);

    return 1;
}

/**
 * Cofactor of matrix.
 * @function cofactor
 * @treturn matrix4
 */
static int modules_matrix4_cofactor(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix4(L, 1);

    lua_settop(L, 0);

    mfloat_t result[MAT4_SIZE];
    mat4_cofactor(result, m0);

    lua_newmatrix4_from_matrix(L, result);

    return 1;
}

/**
 * Inverse of matrix.
 * @function inverse
 * @treturn matrix4
 */
static int modules_matrix4_inverse(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix4(L, 1);

    lua_settop(L, 0);

    mfloat_t result[MAT4_SIZE];
    mat4_inverse(result, m0);

    lua_newmatrix4_from_matrix(L, result);

    return 1;
}

/**
 * Translates matrix in-place.
 * @function translate
 * @tparam vector3.vector3 v0
 * @treturn matrix4
 */

/**
 * Translates matrix in-place.
 * @function translate
 * @tparam number x
 * @tparam number y
 * @tparam number z
 * @treturn matrix4
 */
static int modules_matrix4_translate(lua_State* L) {
    int arg_count = lua_gettop(L);

    mfloat_t* v0 = NULL;
    mfloat_t v1[VEC3_SIZE];

    mfloat_t* m0 = luaL_checkmatrix4(L, 1);

    if (arg_count == 2) {
        v0 = luaL_checkvector3(L, 2);
    }
    else {
        mfloat_t x = luaL_checknumber(L, 2);
        mfloat_t y = luaL_checknumber(L, 3);
        mfloat_t z = luaL_checknumber(L, 4);

        v1[0] = x;
        v1[1] = y;
        v1[2] = z;

        v0 = v1;
    }

    lua_settop(L, 1);

    mat4_translate(m0, m0, v0);

    return 1;
}

/**
 * Scales matrix in-place.
 * @function scale
 * @tparam vector3.vector3 v0
 * @treturn matrix4
 */

/**
 * Scales matrix in-place.
 * @function scale
 * @tparam number x
 * @tparam number y
 * @tparam number z
 * @treturn matrix4
 */
static int modules_matrix4_scale(lua_State* L) {
    int arg_count = lua_gettop(L);

    mfloat_t* v0 = NULL;
    mfloat_t v1[VEC3_SIZE];

    mfloat_t* m0 = luaL_checkmatrix4(L, 1);

    if (arg_count == 2) {
        v0 = luaL_checkvector3(L, 2);
    }
    else {
        mfloat_t x = luaL_checknumber(L, 2);
        mfloat_t y = luaL_checknumber(L, 3);
        mfloat_t z = luaL_checknumber(L, 4);

        v1[0] = x;
        v1[1] = y;
        v1[2] = z;

        v0 = v1;
    }

    lua_settop(L, 1);

    mat4_scale(m0, m0, v0);

    return 1;
}

/**
 * Functions
 * @section Functions
 */

/**
 * Returns a new matrix4
 * @function new
 * @tparam ?number m11
 * @tparam ?number m21
 * @tparam ?number m31
 * @tparam ?number m41
 * @tparam ?number m12
 * @tparam ?number m22
 * @tparam ?number m32
 * @tparam ?number m42
 * @tparam ?number m13
 * @tparam ?number m23
 * @tparam ?number m33
 * @tparam ?number m43
 * @tparam ?number m14
 * @tparam ?number m24
 * @tparam ?number m34
 * @tparam ?number m44
 * @treturn matrix4
 */
static int modules_matrix4_new(lua_State* L) {
    float m11 = (float)luaL_optnumber(L, 1, 0);
    float m21 = (float)luaL_optnumber(L, 2, 0);
    float m31 = (float)luaL_optnumber(L, 3, 0);
    float m41 = (float)luaL_optnumber(L, 4, 0);
    float m12 = (float)luaL_optnumber(L, 5, 0);
    float m22 = (float)luaL_optnumber(L, 6, 0);
    float m32 = (float)luaL_optnumber(L, 7, 0);
    float m42 = (float)luaL_optnumber(L, 8, 0);
    float m13 = (float)luaL_optnumber(L, 9, 0);
    float m23 = (float)luaL_optnumber(L, 10, 0);
    float m33 = (float)luaL_optnumber(L, 11, 0);
    float m43 = (float)luaL_optnumber(L, 12, 0);
    float m14 = (float)luaL_optnumber(L, 13, 0);
    float m24 = (float)luaL_optnumber(L, 14, 0);
    float m34 = (float)luaL_optnumber(L, 15, 0);
    float m44 = (float)luaL_optnumber(L, 16, 0);

    lua_settop(L, 0);

    lua_newmatrix4(L, m11, m21, m31, m41, m12, m22, m32, m42, m13, m23, m33, m43, m14, m24, m34, m44);

    return 1;
}

/**
 * Returns a matrix with all elements set to zero.
 * @function zero
 * @treturn matrix4
 */
static int modules_matrix4_zero(lua_State* L) {
    lua_newmatrix4(L, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    mfloat_t* m0 = luaL_checkmatrix4(L, 1);
    mat4_zero(m0);

    return 1;
}

/**
 * Returns an identity matrix.
 * @function identity
 * @treturn matrix4
 */
static int modules_matrix4_identity(lua_State* L) {
    lua_newmatrix4(L, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    mfloat_t* m0 = luaL_checkmatrix4(L, 1);
    mat4_identity(m0);

    return 1;
}

static int modules_matrix4_rotate_x_axis(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix4(L, 1);
    mfloat_t f = luaL_checknumber(L, 2);

    lua_settop(L, 0);

    mfloat_t result[MAT4_SIZE];
    mat4_assign(result, m0);
    mat4_rotation_x(result, f);

    lua_newmatrix4_from_matrix(L, result);

    return 1;
}

static int modules_matrix4_rotate_y_axis(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix4(L, 1);
    mfloat_t f = luaL_checknumber(L, 2);

    lua_settop(L, 0);

    mfloat_t result[MAT4_SIZE];
    mat4_assign(result, m0);
    mat4_rotation_y(result, f);

    lua_newmatrix4_from_matrix(L, result);

    return 1;
}

static int modules_matrix4_rotate_z_axis(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix4(L, 1);
    mfloat_t f = luaL_checknumber(L, 2);

    lua_settop(L, 0);

    mfloat_t result[MAT4_SIZE];
    mat4_assign(result, m0);
    mat4_rotation_z(result, f);

    lua_newmatrix4_from_matrix(L, result);

    return 1;
}

static int modules_matrix4_rotate_around_axis(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix4(L, 1);
    mfloat_t* v0 = luaL_checkvector3(L, 2);
    mfloat_t f = luaL_checknumber(L, 3);

    lua_settop(L, 0);

    mfloat_t result[MAT4_SIZE];
    mat4_assign(result, m0);
    mat4_rotation_axis(result, v0, f);

    lua_newmatrix4_from_matrix(L, result);

    return 1;
}

/**
 * Creates a rotation matrix
 * @function rotation
 * @tparam quaternion.quaternion q0
 * @treturn matrix4
 */
static int modules_matrix4_rotation_quaternion(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    lua_settop(L, 0);

    mfloat_t result[MAT4_SIZE];
    mat4_rotation_quat(result, q0);

    lua_newmatrix4_from_matrix(L, result);

    return 1;
}

/**
 * Creates a translation matrix
 * @function translation
 * @tparam vector3.vector3 v0
 * @treturn matrix4
 */

/**
 * Creates a translation matrix
 * @function translation
 * @tparam number x
 * @tparam number y
 * @tparam number z
 * @treturn matrix4
 */
static int modules_matrix4_translation(lua_State* L) {
    int arg_count = lua_gettop(L);

    mfloat_t* v0 = NULL;
    mfloat_t v1[VEC3_SIZE];

    if (arg_count == 1) {
        v0 = luaL_checkvector3(L, 1);
    }
    else {
        mfloat_t x = luaL_checknumber(L, 1);
        mfloat_t y = luaL_checknumber(L, 2);
        mfloat_t z = luaL_checknumber(L, 3);

        v1[0] = x;
        v1[1] = y;
        v1[2] = z;

        v0 = v1;
    }

    lua_settop(L, 0);

    modules_matrix4_identity(L);
    mfloat_t* result = luaL_checkmatrix4(L, 1);

    mat4_translation(result, result, v0);

    return 1;
}

/**
 * Creates a scaling matrix
 * @function scaling
 * @tparam vector3.vector3 v0
 * @treturn matrix4
 */

/**
 * Creates a scaling matrix
 * @function scaling
 * @tparam number x
 * @tparam number y
 * @tparam number z
 * @treturn matrix4
 */
static int modules_matrix4_scaling(lua_State* L) {
    int arg_count = lua_gettop(L);

    mfloat_t* v0 = NULL;
    mfloat_t v1[VEC3_SIZE];

    if (arg_count == 1) {
        v0 = luaL_checkvector3(L, 1);
    }
    else {
        mfloat_t x = luaL_checknumber(L, 1);
        mfloat_t y = luaL_checknumber(L, 2);
        mfloat_t z = luaL_checknumber(L, 3);

        v1[0] = x;
        v1[1] = y;
        v1[2] = z;

        v0 = v1;
    }

    lua_settop(L, 0);

    modules_matrix4_identity(L);
    mfloat_t* result = luaL_checkmatrix4(L, 1);

    mat4_scaling(result, result, v0);

    return 1;
}

static int modules_matrix4_multiply(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix4(L, 1);

    // vector4 multiplication
    if (lua_isvector4(L, 2)) {
        mfloat_t* v0 = luaL_checkvector4(L, 2);

        lua_settop(L, 0);

        mfloat_t result[VEC4_SIZE] = { 0.0f, 0.0f, 0.0f, 1.0f };
        vec4_multiply_mat4(result, v0, m0);

        lua_newvector4(L, result[0], result[1], result[2], result[3]);
    }
    // vector3 multiplication
    else if (lua_isvector3(L, 2)) {
        mfloat_t* v0 = luaL_checkvector3(L, 2);

        lua_settop(L, 0);

        mfloat_t v1[VEC4_SIZE] = { v0[0], v0[1], v0[2], 1.0f };
        vec4_multiply_mat4(v1, v1, m0);

        lua_newvector3(L, v1[0], v1[1], v1[2]);
    }
    // Scalar multiplication
    else if (lua_isnumber(L, 2)) {
        mfloat_t f = luaL_checknumber(L, 2);

        lua_settop(L, 0);

        mfloat_t result[MAT4_SIZE];
        mat4_multiply_f(result, m0, f);

        lua_newmatrix4_from_matrix(L, result);
    }
    // Matrix multiplication
    else {
        mfloat_t* m1 = luaL_checkmatrix4(L, 2);

        lua_settop(L, 0);

        mfloat_t result[MAT4_SIZE];
        mat4_multiply(result, m0, m1);

        lua_newmatrix4_from_matrix(L, result);
    }

    return 1;
}

/**
 * Linearly interpolate two matrices.
 * @function lerp
 * @tparam matrix4.matrix4 m0
 * @tparam matrix4.matrix4 m1
 * @tparam number f
 * @treturn matrix4
 */
static int modules_matrix4_lerp(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix4(L, 1);
    mfloat_t* m1 = luaL_checkmatrix4(L, 2);
    mfloat_t f = luaL_checknumber(L, 3);

    lua_settop(L, 0);

    mfloat_t result[MAT4_SIZE];
    mat4_lerp(result, m0, m1, f);

    lua_newmatrix4_from_matrix(L, result);

    return 1;
}

/**
 * Creates a look at transformation.
 * @function look_at
 * @tparam vector3.vector3 position
 * @tparam vector3.vector3 target
 * @tparam vector3.vector3 up
 * @treturn matrix4
 */
static int modules_matrix4_look_at(lua_State* L) {
    mfloat_t* position = luaL_checkvector3(L, 1);
    mfloat_t* target = luaL_checkvector3(L, 2);
    mfloat_t* up = luaL_checkvector3(L, 3);

    lua_settop(L, 0);

    mfloat_t result[MAT4_SIZE];
    mat4_look_at(result, position, target, up);

    lua_newmatrix4_from_matrix(L, result);

    return 1;
}

/**
 * Creates an orthographic view transformation.
 * @function ortho
 * @tparam number l
 * @tparam number r
 * @tparam number b
 * @tparam number t
 * @tparam number n
 * @tparam number f
 * @treturn matrix4
 */
static int modules_matrix4_ortho(lua_State* L) {
    mfloat_t l = luaL_checknumber(L, 1);
    mfloat_t r = luaL_checknumber(L, 2);
    mfloat_t b = luaL_checknumber(L, 3);
    mfloat_t t = luaL_checknumber(L, 4);
    mfloat_t n = luaL_checknumber(L, 5);
    mfloat_t f = luaL_checknumber(L, 6);

    mfloat_t result[MAT4_SIZE];
    mat4_ortho(result, l, r, b, t, n, f);

    lua_newmatrix4_from_matrix(L, result);
    return 1;
}

/**
 * Creates an perspective view transformation.
 * @function perspective
 * @tparam number fov_y
 * @tparam number aspect
 * @tparam number n
 * @tparam number f
 * @treturn matrix4
 */
static int modules_matrix4_perspective(lua_State* L) {
    mfloat_t fov_y = luaL_checknumber(L, 1);
    mfloat_t aspect = luaL_checknumber(L, 2);
    mfloat_t n = luaL_checknumber(L, 3);
    mfloat_t f = luaL_checknumber(L, 4);

    mfloat_t result[MAT4_SIZE];
    mat4_perspective(result, fov_y, aspect, n, f);

    lua_newmatrix4_from_matrix(L, result);
    return 1;
}

/**
 * Creates an perspective view transformation.
 * @function perspective_fov
 * @tparam number fov_y
 * @tparam number w
 * @tparam number h
 * @tparam number n
 * @tparam number f
 * @treturn matrix4
 */
static int modules_matrix4_perspective_fov(lua_State* L) {
    mfloat_t fov_y = luaL_checknumber(L, 1);
    mfloat_t w = luaL_checknumber(L, 2);
    mfloat_t h = luaL_checknumber(L, 3);
    mfloat_t n = luaL_checknumber(L, 4);
    mfloat_t f = luaL_checknumber(L, 5);

    mfloat_t result[MAT4_SIZE];
    mat4_perspective_fov(result, fov_y, w, h, n, f);

    lua_newmatrix4_from_matrix(L, result);
    return 1;
}

/**
 * Creates an perspective view transformation.
 * @function perspective_infinite
 * @tparam number fov_y
 * @tparam number aspect
 * @tparam number n
 * @treturn matrix4
 */
static int modules_matrix4_perspective_infinite(lua_State* L) {
    mfloat_t fov_y = luaL_checknumber(L, 1);
    mfloat_t aspect = luaL_checknumber(L, 2);
    mfloat_t n = luaL_checknumber(L, 3);

    mfloat_t result[MAT4_SIZE];
    mat4_perspective_infinite(result, fov_y, aspect, n);

    lua_newmatrix4_from_matrix(L, result);
    return 1;
}

static const struct luaL_Reg modules_matrix4_functions[] = {
    // Class functions
    {"cofactor", modules_matrix4_cofactor},
    {"determinant", modules_matrix4_determinant},
    {"scale", modules_matrix4_scale},
    {"translate", modules_matrix4_translate},
    {"transpose", modules_matrix4_transpose},
    // Module functions
    {"new", modules_matrix4_new},
    {"zero", modules_matrix4_zero},
    {"identity", modules_matrix4_identity},
    {"rotate_x_axis", modules_matrix4_rotate_x_axis},
    {"rotate_y_axis", modules_matrix4_rotate_y_axis},
    {"rotate_z_axis", modules_matrix4_rotate_z_axis},
    {"rotate_around_axis", modules_matrix4_rotate_around_axis},
    {"rotation", modules_matrix4_rotation_quaternion},
    {"translation", modules_matrix4_translation},
    {"scaling", modules_matrix4_scaling},
    {"inverse", modules_matrix4_inverse},
    {"lerp", modules_matrix4_lerp},
    {"look_at", modules_matrix4_look_at},
    {"ortho", modules_matrix4_ortho},
    {"perspective", modules_matrix4_perspective},
    {"perspective_fov", modules_matrix4_perspective_fov},
    {"perspective_infinite", modules_matrix4_perspective_infinite},
    {NULL, NULL}
};

static int modules_matrix4_meta_index(lua_State* L) {
    mfloat_t* matrix = luaL_checkmatrix4(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_settop(L, 0);

    if (strcmp(key, "m11") == 0) {
        lua_pushnumber(L, matrix[0]);
    }
    else if (strcmp(key, "m21") == 0) {
        lua_pushnumber(L, matrix[1]);
    }
    else if (strcmp(key, "m31") == 0) {
        lua_pushnumber(L, matrix[2]);
    }
    else if (strcmp(key, "m41") == 0) {
        lua_pushnumber(L, matrix[3]);
    }
    else if (strcmp(key, "m12") == 0) {
        lua_pushnumber(L, matrix[4]);
    }
    else if (strcmp(key, "m22") == 0) {
        lua_pushnumber(L, matrix[5]);
    }
    else if (strcmp(key, "m32") == 0) {
        lua_pushnumber(L, matrix[6]);
    }
    else if (strcmp(key, "m42") == 0) {
        lua_pushnumber(L, matrix[7]);
    }
    else if (strcmp(key, "m13") == 0) {
        lua_pushnumber(L, matrix[8]);
    }
    else if (strcmp(key, "m23") == 0) {
        lua_pushnumber(L, matrix[9]);
    }
    else if (strcmp(key, "m33") == 0) {
        lua_pushnumber(L, matrix[10]);
    }
    else if (strcmp(key, "m43") == 0) {
        lua_pushnumber(L, matrix[11]);
    }
    else if (strcmp(key, "m14") == 0) {
        lua_pushnumber(L, matrix[12]);
    }
    else if (strcmp(key, "m24") == 0) {
        lua_pushnumber(L, matrix[13]);
    }
    else if (strcmp(key, "m34") == 0) {
        lua_pushnumber(L, matrix[14]);
    }
    else if (strcmp(key, "m44") == 0) {
        lua_pushnumber(L, matrix[15]);
    }
    else {
        // Check module fields. This enables usage of the colon operator.
        luaL_requiref(L, "matrix4", NULL, false);
        if (lua_type(L, -1) == LUA_TTABLE) {
            lua_getfield(L, -1, key);
        }
        else {
            lua_pushnil(L);
        }
    }

    return 1;
}

static int modules_matrix4_meta_newindex(lua_State* L) {
    mfloat_t* matrix = luaL_checkmatrix4(L, 1);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "m11") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[0] = value;
    }
    else if (strcmp(key, "m21") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[1] = value;
    }
    else if (strcmp(key, "m31") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[2] = value;
    }
    else if (strcmp(key, "m41") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[3] = value;
    }
    else if (strcmp(key, "m12") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[4] = value;
    }
    else if (strcmp(key, "m22") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[5] = value;
    }
    else if (strcmp(key, "m32") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[6] = value;
    }
    else if (strcmp(key, "m42") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[7] = value;
    }
    else if (strcmp(key, "m13") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[8] = value;
    }
    else if (strcmp(key, "m23") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[9] = value;
    }
    else if (strcmp(key, "m33") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[10] = value;
    }
    else if (strcmp(key, "m43") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[11] = value;
    }
    else if (strcmp(key, "m14") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[12] = value;
    }
    else if (strcmp(key, "m24") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[13] = value;
    }
    else if (strcmp(key, "m34") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[14] = value;
    }
    else if (strcmp(key, "m44") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[15] = value;
    }
    else {
        luaL_error(L, "attempt to index a matrix4 value");
    }

    lua_settop(L, 0);

    return 0;
}

static const struct luaL_Reg modules_matrix4_meta_functions[] = {
    {"__index", modules_matrix4_meta_index},
    {"__newindex", modules_matrix4_meta_newindex},
    {"__mul", modules_matrix4_multiply},
    {"__unm", modules_matrix4_negative},
    {NULL, NULL}
};

int luaopen_matrix4(lua_State* L) {
    luaL_newlib(L, modules_matrix4_functions);

    luaL_newmetatable(L, "matrix4");
    luaL_setfuncs(L, modules_matrix4_meta_functions, 0);

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, modules_matrix4_gc);
    lua_settable(L, -3);

    lua_pop(L, 1);

    luaL_newmetatable(L, "matrix4_nogc");
    luaL_setfuncs(L, modules_matrix4_meta_functions, 0);

    lua_pop(L, 1);

    return 1;
}
