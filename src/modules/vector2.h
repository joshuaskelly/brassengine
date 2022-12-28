#ifndef MODULES_VECTOR2_H
#define MODULES_VECTOR2_H

#include <lua/lua.h>

#include <mathc/mathc.h>

mfloat_t* luaL_checkvector2(lua_State* L, int index);

int luaopen_vector2(lua_State* L);

#endif
