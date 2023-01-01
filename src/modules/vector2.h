#ifndef MODULES_VECTOR2_H
#define MODULES_VECTOR2_H

#include <lua/lua.h>

#include <mathc/mathc.h>

/* Checks whether the function argument arg is a vector2 and returns a mfloat_t*. */
mfloat_t* luaL_checkvector2(lua_State* L, int index);

/* Creates and pushes on the stack a new vector2 userdata. */
int lua_newvector2(lua_State* L, float x, float y);

/* Pushes a vector2 onto the stack. Created userdata will not be garbage collected. */
int lua_pushvector2(lua_State* L, mfloat_t* vector);

int luaopen_vector2(lua_State* L);

#endif
