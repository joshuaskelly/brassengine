#ifndef MODULES_VECTOR3_H
#define MODULES_VECTOR3_H

#include <stdbool.h>

#include <lua/lua.h>

#include <mathc/mathc.h>

/* Checks if given arg is a vector3 userdata. */
bool lua_isvector3(lua_State* L, int index);

/* Checks whether given arg is a vector3 and returns a mfloat_t*. */
mfloat_t* luaL_checkvector3(lua_State* L, int index);

/* Creates and pushes on the stack a new vector3 userdata. */
int lua_newvector3(lua_State* L, float x, float y, float z);

/* Pushes a vector3 onto the stack. Created userdata will not be garbage collected. */
int lua_pushvector3(lua_State* L, mfloat_t* vector);

int luaopen_vector3(lua_State* L);

#endif
