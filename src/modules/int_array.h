#ifndef MODULES_INT_ARRAY_H
#define MODULES_INT_ARRAY_H

#include <stdint.h>

#include <lua/lua.h>

#include "../collections/int_array.h"

/* Checks whether the function argument arg is a int array and returns a int_array_t*. */
int_array_t* luaL_checkintarray(lua_State* L, int index);

/* Creates and pushes on the stack a new intarray userdata. */
int lua_newintarray(lua_State* L, size_t size);

/* Pushes a int aray onto the stack. Created userdata will not be garbage collected. */
int lua_pushintarray(lua_State* L, int_array_t* array);

int luaopen_intarray(lua_State* L);

#endif
