#ifndef MODULES_MESH_H
#define MODULES_MESH_H

#include <lua/lua.h>

#include "../renderers/mesh.h"

/* Checks whether the function argument arg is a texture and returns a texure_t*. */
mesh_t* luaL_checkmesh(lua_State* L, int index);

/* Creates and pushes on the stack a new mesh userdata. */
int lua_newmesh(lua_State* L);

/* Pushes a mesh onto the stack. Created userdata will not be garbage collected. */
int lua_pushmesh(lua_State* L, mesh_t* mesh);

int luaopen_mesh(lua_State* L);

#endif
