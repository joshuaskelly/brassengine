#ifndef MODULES_VECTOR4_H
#define MODULES_VECTOR4_H

#include <stdbool.h>

#include <lua/lua.h>

#include <mathc/mathc.h>

/* Checks if given arg is a vector4 userdata. */
bool lua_isvector4(lua_State* L, int index);

/* Checks whether given arg is a vector4 and returns a mfloat_t*. */
mfloat_t* luaL_checkvector4(lua_State* L, int index);

/* Creates and pushes on the stack a new vector4 userdata. */
int lua_newvector4(lua_State* L, float x, float y, float z, float w);

/* Pushes a vector4 onto the stack. Created userdata will not be garbage collected. */
int lua_pushvector4(lua_State* L, mfloat_t* vector);

int luaopen_vector4(lua_State* L);

#endif
