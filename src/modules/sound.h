#ifndef MODULES_SOUND_H
#define MODULES_SOUND_H

#include <lua/lua.h>

#include "../sounds.h"

/** Checks whether the function argument arg is a sound and returns a sound_t*. */
sound_t* luaL_checksound(lua_State* L, int index);

/** Pushes a texture onto the stack. Created userdata will not be garbage collected. */
int lua_pushsound(lua_State* L, sound_t* sound);

int luaopen_sound(lua_State* L);

#endif
