/**
 * Module for rasterizing triangle based meshes.
 * @module mesh
 */

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "mesh.h"

#include "../renderers/mesh.h"

static int module_mesh_render(lua_State* L) {
    mesh_render();

    return 0;
}

static const struct luaL_Reg module_functions[] = {
    {"render", module_mesh_render},
    {NULL, NULL}
};

int luaopen_mesh(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
