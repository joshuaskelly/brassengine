/**
 * Module for rasterizing triangle based meshes.
 * @module mesh
 */
#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "float_array.h"
#include "mesh.h"
#include "texture.h"

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

    if (strcmp(key, "vertices") == 0) {
        lua_pushfloatarray(L, mesh->vertices);
    }
    else if (strcmp(key, "uvs") == 0) {
        lua_pushfloatarray(L, mesh->uvs);
    }
    else if (strcmp(key, "indices") == 0) {
        lua_pushfloatarray(L, mesh->indices);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static int mesh_meta_newindex(lua_State* L) {
    mesh_t* mesh = luaL_checkmesh(L, 1);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "vertices") == 0) {
        if (lua_istable(L, 3)) {
            int size = lua_rawlen(L, 3);

            float_array_resize(mesh->vertices, size);

            for (int i = 1; i <= size; i++) {
                lua_pushinteger(L, i);
                lua_gettable(L, 3);
                mesh->vertices->data[i - 1] = luaL_checknumber(L, -1);
            }
        }
    }
    else if (strcmp(key, "uvs") == 0) {
        if (lua_istable(L, 3)) {
            int size = lua_rawlen(L, 3);

            float_array_resize(mesh->uvs, size);

            for (int i = 1; i <= size; i++) {
                lua_pushinteger(L, i);
                lua_gettable(L, 3);
                mesh->uvs->data[i - 1] = luaL_checknumber(L, -1);
            }
        }
    }
    else if (strcmp(key, "indices") == 0) {
        if (lua_istable(L, 3)) {
            int size = lua_rawlen(L, 3);

            float_array_resize(mesh->indices, size);

            for (int i = 1; i <= size; i++) {
                lua_pushinteger(L, i);
                lua_gettable(L, 3);
                mesh->indices->data[i - 1] = (int)luaL_checknumber(L, -1);
            }

            mesh->index_count = size;
        }
    }
    else {
        luaL_error(L, "attempt to index a mesh value");
    }

    lua_pop(L, -1);

    return 0;
}

static const struct luaL_Reg mesh_meta_functions[] = {
    {"__index", mesh_meta_index},
    {"__newindex", mesh_meta_newindex},
    {NULL, NULL}
};

static int module_mesh_render(lua_State* L) {
    mesh_t* mesh = luaL_checkmesh(L, 1);
    texture_t* texture = luaL_checktexture(L, 2);
    mesh_render(mesh, texture);
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
    luaL_setfuncs(L, mesh_meta_functions, 0);

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, mesh_gc);
    lua_settable(L, -3);

    lua_pop(L, 1);

    return 1;
}
