#ifndef MODULES_FLOAT_ARRAY_H
#define MODULES_FLOAT_ARRAY_H

#include <stdint.h>

#include <lua/lua.h>

#include "../collections/float_array.h"

/* Checks whether the function argument arg is a float array and returns a float_array_t*. */
float_array_t* luaL_checkfloatarray(lua_State* L, int index);

/* Creates and pushes on the stack a new texture userdata. */
int lua_newfloatarray(lua_State* L, size_t size);

/* Pushes a float aray onto the stack. Created userdata will not be garbage collected. */
int lua_pushfloatarray(lua_State* L, float_array_t* array);

int luaopen_floatarray(lua_State* L);

#endif
