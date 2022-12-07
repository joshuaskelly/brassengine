#ifndef MODULES_SOUND_H
#define MODULES_SOUND_H

#include <lua/lua.h>

#include "../sounds.h"

sound_t** luaL_checksound(lua_State* L, int index);

int luaopen_sound(lua_State* L);

#endif
