#ifndef MODULES_MATRIX4_H
#define MODULES_MATRIX4_H

#include <lua/lua.h>

#include <mathc/mathc.h>

/* Checks whether the function argument arg is a matrix4 and returns a mfloat_t*. */
mfloat_t* luaL_checkmatrix4(lua_State* L, int index);

/* Creates and pushes on the stack a new matrix4 userdata. */
int lua_newmatrix4(lua_State* L, float m11, float m21, float m31, float m41, float m12, float m22, float m32, float m42, float m13, float m23, float m33, float m43, float m14, float m24, float m34, float m44);

/* Pushes a matrix4 onto the stack. Created userdata will not be garbage collected. */
int lua_pushmatrix4(lua_State* L, mfloat_t* matrix);

int luaopen_matrix4(lua_State* L);

#endif
