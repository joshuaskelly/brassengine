/**
 * Module for working with matrix3 data.
 * @module matrix3
 */
#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "matrix3.h"
#include "quaternion.h"
#include "vector3.h"

bool lua_ismatrix3(lua_State*L, int index) {
    void* p = luaL_testudata(L, index, "matrix3");
    if (p == NULL) {
        p = luaL_testudata(L, index, "matrix3_nogc");
    }

    return p != NULL;
}

mfloat_t* luaL_checkmatrix3(lua_State* L, int index) {
    mfloat_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);

    handle = (mfloat_t**)luaL_testudata(L, index, "matrix3_nogc");
    if (!handle) {
        handle = (mfloat_t**)luaL_checkudata(L, index, "matrix3");
    }

    return *handle;
}

int lua_newmatrix3(lua_State* L, float m11, float m21, float m31, float m12, float m22, float m32, float m13, float m23, float m33) {
    mfloat_t** handle = (mfloat_t**)lua_newuserdata(L, sizeof(mfloat_t*));
    mfloat_t* m0 = (mfloat_t*)malloc(sizeof(mfloat_t) * MAT3_SIZE);
    m0[0] = m11;
    m0[1] = m21;
    m0[2] = m31;
    m0[3] = m12;
    m0[4] = m22;
    m0[5] = m32;
    m0[6] = m13;
    m0[7] = m23;
    m0[8] = m33;

    *handle = m0;

    luaL_setmetatable(L, "matrix3");

    return 1;
}

int lua_newmatrix3_from_matrix(lua_State* L, mfloat_t* m0) {
    mfloat_t** handle = (mfloat_t**)lua_newuserdata(L, sizeof(mfloat_t*));
    mfloat_t* m1 = (mfloat_t*)malloc(sizeof(mfloat_t) * MAT3_SIZE);
    m1[0] = m0[0];
    m1[1] = m0[1];
    m1[2] = m0[2];
    m1[3] = m0[3];
    m1[4] = m0[4];
    m1[5] = m0[5];
    m1[6] = m0[6];
    m1[7] = m0[7];
    m1[8] = m0[8];

    *handle = m1;

    luaL_setmetatable(L, "matrix3");

    return 1;
}

int lua_pushmatrix3(lua_State* L, mfloat_t* matrix) {
    mfloat_t** handle = (mfloat_t**)lua_newuserdata(L, sizeof(mfloat_t*));
    *handle = matrix;

    luaL_setmetatable(L, "matrix3_nogc");

    return 1;
}

static int matrix3_gc(lua_State* L) {
    mfloat_t** handle = lua_touserdata(L, 1);
    free(*handle);
    *handle = NULL;

    return 0;
}

/**
 * matrix3 class
 * @type matrix3
 */

/** @tfield number m11 */
/** @tfield number m21 */
/** @tfield number m31 */
/** @tfield number m12 */
/** @tfield number m22 */
/** @tfield number m32 */
/** @tfield number m13 */
/** @tfield number m23 */
/** @tfield number m33 */

/**
 * Determinant of matrix.
 * @function determinant
 * @treturn number float
 */
static int matrix3_determinant(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix3(L, 1);
    float determinant = mat3_determinant(m0);

    lua_settop(L, 0);

    lua_pushnumber(L, determinant);

    return 1;
}

/**
 * Negates a matrix.
 * @function negative
 * @treturn matrix3
 */
static int matrix3_negative(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix3(L, 1);

    lua_settop(L, 0);

    mfloat_t result[MAT3_SIZE];
    mat3_negative(result, m0);

    lua_newmatrix3_from_matrix(L, result);

    return 1;
}

/**
 * Transpose of matrix.
 * @function transpose
 * @treturn matrix3
 */
static int matrix3_transpose(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix3(L, 1);

    lua_settop(L, 0);

    mfloat_t result[MAT3_SIZE];
    mat3_transpose(result, m0);

    lua_newmatrix3_from_matrix(L, result);

    return 1;
}

/**
 * Cofactor of matrix.
 * @function cofactor
 * @treturn matrix3
 */
static int matrix3_cofactor(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix3(L, 1);

    lua_settop(L, 0);

    mfloat_t result[MAT3_SIZE];
    mat3_cofactor(result, m0);

    lua_newmatrix3_from_matrix(L, result);

    return 1;
}

/**
 * Inverse of matrix.
 * @function inverse
 * @treturn matrix3
 */
static int matrix3_inverse(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix3(L, 1);

    lua_settop(L, 0);

    mfloat_t result[MAT3_SIZE];
    mat3_inverse(result, m0);

    lua_newmatrix3_from_matrix(L, result);

    return 1;
}

/**
 * Scales matrix in-place.
 * @function scale
 * @tparam vector3.vector3 v0
 * @treturn matrix3
 */

/**
 * Scales matrix in-place.
 * @function scale
 * @tparam number x
 * @tparam number y
 * @tparam number z
 * @treturn matrix3
 */
static int matrix3_scale(lua_State* L) {
    int arg_count = lua_gettop(L);

    mfloat_t* v0 = NULL;
    mfloat_t v1[VEC3_SIZE];

    mfloat_t* m0 = luaL_checkmatrix3(L, 1);

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

    mat3_scale(m0, m0, v0);

    return 1;
}

/**
 * Functions
 * @section Functions
 */

/**
 * Returns a new matrix3
 * @function new
 * @tparam ?number m11
 * @tparam ?number m21
 * @tparam ?number m31
 * @tparam ?number m12
 * @tparam ?number m22
 * @tparam ?number m32
 * @tparam ?number m13
 * @tparam ?number m23
 * @tparam ?number m33
 * @treturn matrix3
 */
static int matrix3_new(lua_State* L) {
    float m11 = (float)luaL_optnumber(L, 1, 0);
    float m21 = (float)luaL_optnumber(L, 2, 0);
    float m31 = (float)luaL_optnumber(L, 3, 0);
    float m12 = (float)luaL_optnumber(L, 4, 0);
    float m22 = (float)luaL_optnumber(L, 5, 0);
    float m32 = (float)luaL_optnumber(L, 6, 0);
    float m13 = (float)luaL_optnumber(L, 7, 0);
    float m23 = (float)luaL_optnumber(L, 8, 0);
    float m33 = (float)luaL_optnumber(L, 9, 0);

    lua_settop(L, 0);

    lua_newmatrix3(L, m11, m21, m31, m12, m22, m32, m13, m23, m33);

    return 1;
}

/**
 * Returns a matrix with all elements set to zero.
 * @function zero
 * @treturn matrix3
 */
static int matrix3_zero(lua_State* L) {
    lua_newmatrix3(L, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    mfloat_t* m0 = luaL_checkmatrix3(L, 1);
    mat3_zero(m0);

    return 1;
}

/**
 * Returns an identity matrix.
 * @function identity
 * @treturn matrix3
 */
static int matrix3_identity(lua_State* L) {
    lua_newmatrix3(L, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    mfloat_t* m0 = luaL_checkmatrix3(L, 1);
    mat3_identity(m0);

    return 1;
}

static int matrix3_rotate_x_axis(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix3(L, 1);
    mfloat_t f = luaL_checknumber(L, 2);

    lua_settop(L, 0);

    mfloat_t result[MAT3_SIZE];
    mat3_assign(result, m0);
    mat3_rotation_x(result, f);

    lua_newmatrix3_from_matrix(L, result);

    return 1;
}

static int matrix3_rotate_y_axis(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix3(L, 1);
    mfloat_t f = luaL_checknumber(L, 2);

    lua_settop(L, 0);

    mfloat_t result[MAT3_SIZE];
    mat3_assign(result, m0);
    mat3_rotation_y(result, f);

    lua_newmatrix3_from_matrix(L, result);

    return 1;
}

static int matrix3_rotate_z_axis(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix3(L, 1);
    mfloat_t f = luaL_checknumber(L, 2);

    lua_settop(L, 0);

    mfloat_t result[MAT3_SIZE];
    mat3_assign(result, m0);
    mat3_rotation_z(result, f);

    lua_newmatrix3_from_matrix(L, result);

    return 1;
}

static int matrix3_rotate_around_axis(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix3(L, 1);
    mfloat_t* v0 = luaL_checkvector3(L, 2);
    mfloat_t f = luaL_checknumber(L, 3);

    lua_settop(L, 0);

    mfloat_t result[MAT3_SIZE];
    mat3_assign(result, m0);
    mat3_rotation_axis(result, v0, f);

    lua_newmatrix3_from_matrix(L, result);

    return 1;
}

/**
 * Creates a rotation matrix
 * @function rotation
 * @tparam quaternion.quaternion q0
 * @treturn matrix3
 */
static int matrix3_rotation_quaternion(lua_State* L) {
    mfloat_t* q0 = luaL_checkquaternion(L, 1);

    lua_settop(L, 0);

    mfloat_t result[MAT3_SIZE];
    mat3_rotation_quat(result, q0);

    lua_newmatrix3_from_matrix(L, result);

    return 1;
}

/**
 * Creates a scaling matrix
 * @function scaling
 * @tparam vector3.vector3 v0
 * @treturn matrix3
 */

/**
 * Creates a scaling matrix
 * @function scaling
 * @tparam number x
 * @tparam number y
 * @tparam number z
 * @treturn matrix3
 */
static int matrix3_scaling(lua_State* L) {
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

    matrix3_identity(L);
    mfloat_t* result = luaL_checkmatrix3(L, 1);

    mat3_scaling(result, v0);

    return 1;
}

static int matrix3_multiply(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix3(L, 1);

    // vector3 multiplication
    if (lua_isvector3(L, 2)) {
        mfloat_t* v0 = luaL_checkvector3(L, 2);

        lua_settop(L, 0);

        mfloat_t v1[VEC3_SIZE] = { v0[0], v0[1], v0[2] };
        vec3_multiply_mat3(v1, v1, m0);

        lua_newvector3(L, v1[0], v1[1], v1[2]);
    }
    // Scalar multiplication
    else if (lua_isnumber(L, 2)) {
        mfloat_t f = luaL_checknumber(L, 2);

        lua_settop(L, 0);

        mfloat_t result[MAT3_SIZE];
        mat3_multiply_f(result, m0, f);

        lua_newmatrix3_from_matrix(L, result);
    }
    // Matrix multiplication
    else {
        mfloat_t* m1 = luaL_checkmatrix3(L, 2);

        lua_settop(L, 0);

        mfloat_t result[MAT3_SIZE];
        mat3_multiply(result, m0, m1);

        lua_newmatrix3_from_matrix(L, result);
    }

    return 1;
}

/**
 * Linearly interpolate two matrices.
 * @function lerp
 * @tparam matrix3.matrix3 m0
 * @tparam matrix3.matrix3 m1
 * @tparam number f
 * @treturn matrix3
 */
static int matrix3_lerp(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix3(L, 1);
    mfloat_t* m1 = luaL_checkmatrix3(L, 2);
    mfloat_t f = luaL_checknumber(L, 3);

    lua_settop(L, 0);

    mfloat_t result[MAT3_SIZE];
    mat3_lerp(result, m0, m1, f);

    lua_newmatrix3_from_matrix(L, result);

    return 1;
}

static const struct luaL_Reg module_functions[] = {
    // Class functions
    {"cofactor", matrix3_cofactor},
    {"determinant", matrix3_determinant},
    {"scale", matrix3_scale},
    {"transpose", matrix3_transpose},
    // Module functions
    {"new", matrix3_new},
    {"zero", matrix3_zero},
    {"identity", matrix3_identity},
    {"rotate_x_axis", matrix3_rotate_x_axis},
    {"rotate_y_axis", matrix3_rotate_y_axis},
    {"rotate_z_axis", matrix3_rotate_z_axis},
    {"rotate_around_axis", matrix3_rotate_around_axis},
    {"rotation", matrix3_rotation_quaternion},
    {"scaling", matrix3_scaling},
    {"inverse", matrix3_inverse},
    {"lerp", matrix3_lerp},
    {NULL, NULL}
};

static int matrix3_meta_index(lua_State* L) {
    mfloat_t* matrix = luaL_checkmatrix3(L, 1);
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
    else if (strcmp(key, "m12") == 0) {
        lua_pushnumber(L, matrix[3]);
    }
    else if (strcmp(key, "m22") == 0) {
        lua_pushnumber(L, matrix[4]);
    }
    else if (strcmp(key, "m32") == 0) {
        lua_pushnumber(L, matrix[5]);
    }
    else if (strcmp(key, "m13") == 0) {
        lua_pushnumber(L, matrix[6]);
    }
    else if (strcmp(key, "m23") == 0) {
        lua_pushnumber(L, matrix[7]);
    }
    else if (strcmp(key, "m33") == 0) {
        lua_pushnumber(L, matrix[8]);
    }
    else {
        // Check module fields. This enables usage of the colon operator.
        luaL_requiref(L, "matrix3", NULL, false);
        if (lua_type(L, -1) == LUA_TTABLE) {
            lua_getfield(L, -1, key);
        }
        else {
            lua_pushnil(L);
        }
    }

    return 1;
}

static int matrix3_meta_newindex(lua_State* L) {
    mfloat_t* matrix = luaL_checkmatrix3(L, 1);
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
    else if (strcmp(key, "m12") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[3] = value;
    }
    else if (strcmp(key, "m22") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[4] = value;
    }
    else if (strcmp(key, "m32") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[5] = value;
    }
    else if (strcmp(key, "m13") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[6] = value;
    }
    else if (strcmp(key, "m23") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[7] = value;
    }
    else if (strcmp(key, "m33") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[8] = value;
    }
    else {
        luaL_error(L, "attempt to index a matrix3 value");
    }

    lua_settop(L, 0);

    return 0;
}

static const struct luaL_Reg meta_functions[] = {
    {"__index", matrix3_meta_index},
    {"__newindex", matrix3_meta_newindex},
    {"__mul", matrix3_multiply},
    {"__unm", matrix3_negative},
    {NULL, NULL}
};

int luaopen_matrix3(lua_State* L) {
    luaL_newlib(L, module_functions);

    luaL_newmetatable(L, "matrix3");
    luaL_setfuncs(L, meta_functions, 0);

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, matrix3_gc);
    lua_settable(L, -3);

    lua_pop(L, 1);

    luaL_newmetatable(L, "matrix3_nogc");
    luaL_setfuncs(L, meta_functions, 0);

    lua_pop(L, 1);

    return 1;
}
