#ifndef MODULES_TEXTURE_H
#define MODULES_TEXTURE_H

#include <lua/lua.h>

#include "../graphics.h"

texture_t* luaL_checktexture(lua_State* L, int index);

int lua_pushtexture(lua_State* L, texture_t* texture);

int luaopen_texture(lua_State* L);

#endif
