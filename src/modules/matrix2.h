#ifndef MODULES_MATRIX2_H
#define MODULES_MATRIX2_H

#include <stdbool.h>

#include <lua/lua.h>

#include <mathc/mathc.h>

/* Checks if given arg is a matrix2 userdata. */
bool lua_ismatrix2(lua_State*L, int index);

/* Checks whether the function argument arg is a matrix2 and returns a mfloat_t*. */
mfloat_t* luaL_checkmatrix2(lua_State* L, int index);

/* Creates and pushes on the stack a new matrix2 userdata. */
int lua_newmatrix2(lua_State* L, float m11, float m21, float m12, float m22);

/* Pushes a matrix2 onto the stack. Created userdata will not be garbage collected. */
int lua_pushmatrix2(lua_State* L, mfloat_t* matrix);

int luaopen_matrix2(lua_State* L);

#endif
