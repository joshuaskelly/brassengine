#ifndef MODULES_QUATERNION_H
#define MODULES_QUATERNION_H

#include <lua/lua.h>

#include <mathc/mathc.h>

/* Checks whether the function argument arg is a quaternion and returns a mfloat_t*. */
mfloat_t* luaL_checkquaternion(lua_State* L, int index);

/* Creates and pushes on the stack a new quaternion userdata. */
int lua_newquaternion(lua_State* L, float x, float y, float z, float w);

/* Pushes a quaternion onto the stack. Created userdata will not be garbage collected. */
int lua_pushquaternion(lua_State* L, mfloat_t* matrix);

int luaopen_quaternion(lua_State* L);

#endif
