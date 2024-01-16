#ifndef MODULES_MATRIX3_H
#define MODULES_MATRIX3_H

#include <stdbool.h>

#include <lua/lua.h>

#include <mathc/mathc.h>

/* Checks if given arg is a matrix4 userdata. */
bool lua_ismatrix3(lua_State*L, int index);

/* Checks whether the function argument arg is a matrix4 and returns a mfloat_t*. */
mfloat_t* luaL_checkmatrix3(lua_State* L, int index);

/* Creates and pushes on the stack a new matrix3 userdata. */
int lua_newmatrix3(lua_State* L, float m11, float m21, float m31, float m12, float m22, float m32, float m13, float m23, float m33);

/* Pushes a matrix3 onto the stack. Created userdata will not be garbage collected. */
int lua_pushmatrix3(lua_State* L, mfloat_t* matrix);

int luaopen_matrix3(lua_State* L);

#endif
