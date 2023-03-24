/**
 * Module for rasterizing triangle based meshes.
 * @module mesh
 */

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "mesh.h"

#include "../assets.h"
#include "../graphics.h"
#include "../renderers/mesh.h"

// Temp data
static mfloat_t vertices[] = {20, 20, 100, 20, 20, 100, 100, 100};
static mfloat_t uvs[] = {0, 0, 1, 0, 0, 1, 1, 1};
static int indices[] = {0, 1, 2, 3, 2, 1};

static mesh_t m1 = {
    .vertices = vertices,
    .uvs = uvs,
    .indices = indices,
    .index_count = 6
};

static int module_mesh_render(lua_State* L) {
    texture_t* tex = assets_get_texture("texture.gif");
    mesh_render(&m1, tex);
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
