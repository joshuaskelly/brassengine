/**
 * Module for rasterizing triangle based meshes.
 * @module mesh
 */
#include <stdlib.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "mesh.h"

#include "../assets.h"
#include "../graphics.h"
#include "../renderers/mesh.h"

mesh_t* luaL_checkmesh(lua_State* L, int index) {
    mesh_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);

    // Ensure we have correct userdata
    handle = (mesh_t**)luaL_testudata(L, index, "mesh_nogc");
    if (!handle) {
        handle = (mesh_t**)luaL_checkudata(L, index, "mesh");
    }

    return *handle;
}

int lua_newmesh(lua_State* L) {
    mesh_t** handle = (mesh_t**)lua_newuserdata(L, sizeof(mesh_t*));
    *handle = mesh_mesh_new();
    luaL_setmetatable(L, "mesh");

    return 1;
}

int lua_pushmesh(lua_State* L, mesh_t* mesh) {
    mesh_t** handle = (mesh_t**)lua_newuserdata(L, sizeof(mesh_t*));
    *handle = mesh;
    luaL_setmetatable(L, "mesh_nogc");

    return 1;
}

static int mesh_gc(lua_State* L) {
    mesh_t** mesh = lua_touserdata(L, 1);
    mesh_mesh_free(*mesh);

    return 0;
}

static int mesh_meta_index(lua_State* L) {
    mesh_t* mesh = luaL_checkmesh(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_pop(L, -1);

    // if (strcmp(key, "position") == 0) {
    // }
    // else {
    //     lua_pushnil(L);
    // }

    return 1;
}

static int mesh_meta_newindex(lua_State* L) {
    mesh_t* mesh = luaL_checkmesh(L, 1);
    const char* key = luaL_checkstring(L, 2);

    // if (strncmp(key, "position", 8) == 0) {
    // }
    // else {
    //     luaL_error(L, "attempt to index a mesh value");
    // }

    lua_pop(L, -1);

    return 0;
}

static const struct luaL_Reg mesh_meta_functions[] = {
    {"__index", mesh_meta_index},
    {"__newindex", mesh_meta_newindex},
    {NULL, NULL}
};

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
    {"new_mesh", lua_newmesh},
    {"render", module_mesh_render},
    {NULL, NULL}
};

int luaopen_mesh(lua_State* L) {
    luaL_newlib(L, module_functions);

    luaL_newmetatable(L, "mesh");
    luaL_newlib(L, mesh_meta_functions);
    lua_setfield(L, -2, "__index");

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, mesh_gc);
    lua_settable(L, -3);

    lua_pop(L, 1);

    return 1;
}
