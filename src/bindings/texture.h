#ifndef BINDINGS_TEXTURE_H
#define BINDINGS_TEXTURE_H

#include <lua/lua.h>

#include "../graphics.h"

texture_t** luaL_checktexture(lua_State* L, int index);

int open_texture_module(lua_State* L);

#endif
