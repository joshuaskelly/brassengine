/**
 * Module for working with matrix2 data.
 * @module matrix2
 */
#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "matrix2.h"
#include "vector2.h"

bool lua_ismatrix2(lua_State*L, int index) {
    void* p = luaL_testudata(L, index, "matrix2");
    if (p == NULL) {
        p = luaL_testudata(L, index, "matrix2_nogc");
    }

    return p != NULL;
}

mfloat_t* luaL_checkmatrix2(lua_State* L, int index) {
    mfloat_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);

    handle = (mfloat_t**)luaL_testudata(L, index, "matrix2_nogc");
    if (!handle) {
        handle = (mfloat_t**)luaL_checkudata(L, index, "matrix2");
    }

    return *handle;
}

int lua_newmatrix2(lua_State* L, float m11, float m21, float m12, float m22) {
    mfloat_t** handle = (mfloat_t**)lua_newuserdata(L, sizeof(mfloat_t*));
    mfloat_t* m0 = (mfloat_t*)malloc(sizeof(mfloat_t) * MAT2_SIZE);
    m0[0] = m11;
    m0[1] = m21;
    m0[2] = m12;
    m0[3] = m22;

    *handle = m0;

    luaL_setmetatable(L, "matrix2");

    return 1;
}

int lua_newmatrix2_from_matrix(lua_State* L, mfloat_t* m0) {
    mfloat_t** handle = (mfloat_t**)lua_newuserdata(L, sizeof(mfloat_t*));
    mfloat_t* m1 = (mfloat_t*)malloc(sizeof(mfloat_t) * MAT2_SIZE);
    m1[0] = m0[0];
    m1[1] = m0[1];
    m1[2] = m0[2];
    m1[3] = m0[3];

    *handle = m1;

    luaL_setmetatable(L, "matrix2");

    return 1;
}

int lua_pushmatrix2(lua_State* L, mfloat_t* matrix) {
    mfloat_t** handle = (mfloat_t**)lua_newuserdata(L, sizeof(mfloat_t*));
    *handle = matrix;

    luaL_setmetatable(L, "matrix2_nogc");

    return 1;
}

static int modules_matrix2_gc(lua_State* L) {
    mfloat_t** handle = lua_touserdata(L, 1);
    free(*handle);
    *handle = NULL;

    return 0;
}

/**
 * matrix2 class
 * @type matrix2
 */

/** @tfield number m11 */
/** @tfield number m21 */
/** @tfield number m12 */
/** @tfield number m22 */

/**
 * Determinant of matrix.
 * @function determinant
 * @treturn number float
 */
static int modules_matrix2_determinant(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix2(L, 1);
    float determinant = mat2_determinant(m0);

    lua_settop(L, 0);

    lua_pushnumber(L, determinant);

    return 1;
}

/**
 * Negates a matrix.
 * @function negative
 * @treturn matrix2
 */
static int modules_matrix2_negative(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix2(L, 1);

    lua_settop(L, 0);

    mfloat_t result[MAT2_SIZE];
    mat2_negative(result, m0);

    lua_newmatrix2_from_matrix(L, result);

    return 1;
}

/**
 * Transpose of matrix.
 * @function transpose
 * @treturn matrix2
 */
static int modules_matrix2_transpose(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix2(L, 1);

    lua_settop(L, 0);

    mfloat_t result[MAT2_SIZE];
    mat2_transpose(result, m0);

    lua_newmatrix2_from_matrix(L, result);

    return 1;
}

/**
 * Cofactor of matrix.
 * @function cofactor
 * @treturn matrix2
 */
static int modules_matrix2_cofactor(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix2(L, 1);

    lua_settop(L, 0);

    mfloat_t result[MAT2_SIZE];
    mat2_cofactor(result, m0);

    lua_newmatrix2_from_matrix(L, result);

    return 1;
}

/**
 * Inverse of matrix.
 * @function inverse
 * @treturn matrix2
 */
static int modules_matrix2_inverse(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix2(L, 1);

    lua_settop(L, 0);

    mfloat_t result[MAT2_SIZE];
    mat2_inverse(result, m0);

    lua_newmatrix2_from_matrix(L, result);

    return 1;
}

/**
 * Scales matrix in-place.
 * @function scale
 * @tparam vector2.vector2 v0
 * @treturn matrix2
 */

/**
 * Scales matrix in-place.
 * @function scale
 * @tparam number x
 * @tparam number y
 * @tparam number z
 * @treturn matrix2
 */
static int modules_matrix2_scale(lua_State* L) {
    int arg_count = lua_gettop(L);

    mfloat_t* v0 = NULL;
    mfloat_t v1[VEC2_SIZE];

    mfloat_t* m0 = luaL_checkmatrix2(L, 1);

    if (arg_count == 2) {
        v0 = luaL_checkvector2(L, 2);
    }
    else {
        mfloat_t x = luaL_checknumber(L, 2);
        mfloat_t y = luaL_checknumber(L, 3);

        v1[0] = x;
        v1[1] = y;

        v0 = v1;
    }

    lua_settop(L, 1);

    mat2_scale(m0, m0, v0);

    return 1;
}

/**
 * Functions
 * @section Functions
 */

/**
 * Returns a new matrix2
 * @function new
 * @tparam ?number m11
 * @tparam ?number m21
 * @tparam ?number m12
 * @tparam ?number m22
 * @treturn matrix2
 */
static int modules_matrix2_new(lua_State* L) {
    float m11 = (float)luaL_optnumber(L, 1, 0);
    float m21 = (float)luaL_optnumber(L, 2, 0);
    float m12 = (float)luaL_optnumber(L, 3, 0);
    float m22 = (float)luaL_optnumber(L, 4, 0);

    lua_settop(L, 0);

    lua_newmatrix2(L, m11, m21, m12, m22);

    return 1;
}

/**
 * Returns a matrix with all elements set to zero.
 * @function zero
 * @treturn matrix2
 */
static int modules_matrix2_zero(lua_State* L) {
    lua_newmatrix2(L, 0, 0, 0, 0);
    mfloat_t* m0 = luaL_checkmatrix2(L, 1);
    mat2_zero(m0);

    return 1;
}

/**
 * Returns an identity matrix.
 * @function identity
 * @treturn matrix2
 */
static int modules_matrix2_identity(lua_State* L) {
    lua_newmatrix2(L, 0, 0, 0, 0);
    mfloat_t* m0 = luaL_checkmatrix2(L, 1);
    mat2_identity(m0);

    return 1;
}

static int modules_matrix2_rotate_z_axis(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix2(L, 1);
    mfloat_t f = luaL_checknumber(L, 2);

    lua_settop(L, 0);

    mfloat_t result[MAT2_SIZE];
    mat2_assign(result, m0);
    mat2_rotation_z(result, f);

    lua_newmatrix2_from_matrix(L, result);

    return 1;
}

/**
 * Creates a scaling matrix
 * @function scaling
 * @tparam vector2.vector2 v0
 * @treturn matrix2
 */

/**
 * Creates a scaling matrix
 * @function scaling
 * @tparam number x
 * @tparam number y
 * @tparam number z
 * @treturn matrix2
 */
static int modules_matrix2_scaling(lua_State* L) {
    int arg_count = lua_gettop(L);

    mfloat_t* v0 = NULL;
    mfloat_t v1[VEC2_SIZE];

    if (arg_count == 1) {
        v0 = luaL_checkvector2(L, 1);
    }
    else {
        mfloat_t x = luaL_checknumber(L, 1);
        mfloat_t y = luaL_checknumber(L, 2);

        v1[0] = x;
        v1[1] = y;

        v0 = v1;
    }

    lua_settop(L, 0);

    modules_matrix2_identity(L);
    mfloat_t* result = luaL_checkmatrix2(L, 1);

    mat2_scaling(result, v0);

    return 1;
}

static int modules_matrix2_multiply(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix2(L, 1);

    // vector2 multiplication
    if (lua_isvector2(L, 2)) {
        mfloat_t* v0 = luaL_checkvector2(L, 2);

        lua_settop(L, 0);

        mfloat_t v1[VEC2_SIZE] = { v0[0], v0[1] };
        vec2_multiply_mat2(v1, v1, m0);

        lua_newvector2(L, v1[0], v1[1]);
    }
    // Scalar multiplication
    else if (lua_isnumber(L, 2)) {
        mfloat_t f = luaL_checknumber(L, 2);

        lua_settop(L, 0);

        mfloat_t result[MAT2_SIZE];
        mat2_multiply_f(result, m0, f);

        lua_newmatrix2_from_matrix(L, result);
    }
    // Matrix multiplication
    else {
        mfloat_t* m1 = luaL_checkmatrix2(L, 2);

        lua_settop(L, 0);

        mfloat_t result[MAT2_SIZE];
        mat2_multiply(result, m0, m1);

        lua_newmatrix2_from_matrix(L, result);
    }

    return 1;
}

/**
 * Linearly interpolate two matrices.
 * @function lerp
 * @tparam matrix2.matrix2 m0
 * @tparam matrix2.matrix2 m1
 * @tparam number f
 * @treturn matrix2
 */
static int modules_matrix2_lerp(lua_State* L) {
    mfloat_t* m0 = luaL_checkmatrix2(L, 1);
    mfloat_t* m1 = luaL_checkmatrix2(L, 2);
    mfloat_t f = luaL_checknumber(L, 3);

    lua_settop(L, 0);

    mfloat_t result[MAT2_SIZE];
    mat2_lerp(result, m0, m1, f);

    lua_newmatrix2_from_matrix(L, result);

    return 1;
}

static const struct luaL_Reg modules_matrix2_functions[] = {
    // Class functions
    {"cofactor", modules_matrix2_cofactor},
    {"determinant", modules_matrix2_determinant},
    {"scale", modules_matrix2_scale},
    {"transpose", modules_matrix2_transpose},
    // Module functions
    {"new", modules_matrix2_new},
    {"zero", modules_matrix2_zero},
    {"identity", modules_matrix2_identity},
    {"rotate_z_axis", modules_matrix2_rotate_z_axis},
    {"scaling", modules_matrix2_scaling},
    {"inverse", modules_matrix2_inverse},
    {"lerp", modules_matrix2_lerp},
    {NULL, NULL}
};

static int modules_matrix2_meta_index(lua_State* L) {
    mfloat_t* matrix = luaL_checkmatrix2(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_settop(L, 0);

    if (strcmp(key, "m11") == 0) {
        lua_pushnumber(L, matrix[0]);
    }
    else if (strcmp(key, "m21") == 0) {
        lua_pushnumber(L, matrix[1]);
    }
    else if (strcmp(key, "m12") == 0) {
        lua_pushnumber(L, matrix[2]);
    }
    else if (strcmp(key, "m22") == 0) {
        lua_pushnumber(L, matrix[3]);
    }
    else {
        // Check module fields. This enables usage of the colon operator.
        luaL_requiref(L, "matrix2", NULL, false);
        if (lua_type(L, -1) == LUA_TTABLE) {
            lua_getfield(L, -1, key);
        }
        else {
            lua_pushnil(L);
        }
    }

    return 1;
}

static int modules_matrix2_meta_newindex(lua_State* L) {
    mfloat_t* matrix = luaL_checkmatrix2(L, 1);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "m11") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[0] = value;
    }
    else if (strcmp(key, "m21") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[1] = value;
    }
    else if (strcmp(key, "m12") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[2] = value;
    }
    else if (strcmp(key, "m22") == 0) {
        float value = (float)luaL_checknumber(L, 3);
        matrix[3] = value;
    }
    else {
        luaL_error(L, "attempt to index a matrix2 value");
    }

    lua_settop(L, 0);

    return 0;
}

static const struct luaL_Reg modules_matrix2_meta_functions[] = {
    {"__index", modules_matrix2_meta_index},
    {"__newindex", modules_matrix2_meta_newindex},
    {"__mul", modules_matrix2_multiply},
    {"__unm", modules_matrix2_negative},
    {NULL, NULL}
};

int luaopen_matrix2(lua_State* L) {
    luaL_newlib(L, modules_matrix2_functions);

    luaL_newmetatable(L, "matrix2");
    luaL_setfuncs(L, modules_matrix2_meta_functions, 0);

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, modules_matrix2_gc);
    lua_settable(L, -3);

    lua_pop(L, 1);

    luaL_newmetatable(L, "matrix2_nogc");
    luaL_setfuncs(L, modules_matrix2_meta_functions, 0);

    lua_pop(L, 1);

    return 1;
}
