#ifndef MODULES_TEXTURE_H
#define MODULES_TEXTURE_H

#include <lua/lua.h>

#include "../graphics.h"

/* Checks whether the function argument arg is a texture and returns a texure_t*. */
texture_t* luaL_checktexture(lua_State* L, int index);

/* If function argument is a texture, return it. If argument is absent or nil, return default_. Otherwise raises an error. */
texture_t* luaL_opttexture(lua_State* L, int index, texture_t* default_);

/* Creates and pushes on the stack a new texture userdata. */
int lua_newtexture(lua_State* L, int width, int height);

/* Pushes a texture onto the stack. Created userdata will not be garbage collected. */
int lua_pushtexture(lua_State* L, texture_t* texture);

int luaopen_texture(lua_State* L);

#endif
