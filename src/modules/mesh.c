/**
 * Module for Mesh renderer
 *
 * @module mesh
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "matrix4.h"
#include "texture.h"

#include "../renderers/mesh.h"

static mesh_mesh_t* luaL_checkmeshmesh(lua_State* L, int index) {
    mesh_mesh_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    handle = (mesh_mesh_t**)luaL_checkudata(L, index, "mesh_mesh");

    if (!handle) {
        luaL_typeerror(L, index, "mesh_mesh");
    }

    return *handle;
}

static int lua_newmeshmesh(lua_State* L) {
    mesh_mesh_t** handle = (mesh_mesh_t**)lua_newuserdata(L, sizeof(mesh_mesh_t*));
    *handle = mesh_mesh_new();
    luaL_setmetatable(L, "mesh_mesh");

    return 1;
}

static int modules_mesh_mesh_free(lua_State* L) {
    mesh_mesh_t** handle = lua_touserdata(L, 1);
    mesh_mesh_free(*handle);
    *handle = NULL;

    return 0;
}

/**
 * @type Mesh
 */

/**
 * Creates a mesh object.
 * @function Mesh:new
 * @treturn Mesh
 */
static int modules_mesh_mesh_new(lua_State* L) {
    return lua_newmeshmesh(L);
}

static int modules_mesh_mesh_meta_index(lua_State* L) {
    mesh_mesh_t* mesh = luaL_checkmeshmesh(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_settop(L, 0);

    if (strcmp(key, "indices") == 0) {
        lua_newtable(L);

        for (int i = 0; i < mesh->triangle_count * 3; i++) {
            lua_pushinteger(L, i + 1);
            lua_pushinteger(L, mesh->indices[i] + 1);
            lua_settable(L, -3);
        }
    }
    else if (strcmp(key, "vertices") == 0) {
        lua_newtable(L);

        for (int i = 0; i < mesh->vertex_count; i++) {
            lua_pushinteger(L, i + 1);
            lua_pushnumber(L, mesh->vertices[i]);
            lua_settable(L, -3);
        }
    }
    else if (strcmp(key, "normals") == 0) {
        lua_newtable(L);

        for (int i = 0; i < mesh->triangle_count * 3; i++) {
            lua_pushinteger(L, i + 1);
            lua_pushnumber(L, mesh->normals[i]);
            lua_settable(L, -3);
        }
    }
    else if (strcmp(key, "colors") == 0) {
        lua_newtable(L);

        for (int i = 0; i < mesh->triangle_count; i++) {
            lua_pushinteger(L, i + 1);
            lua_pushnumber(L, mesh->colors[i]);
            lua_settable(L, -3);
        }
    }
    else {
        luaL_requiref(L, "mesh", NULL, false);
        lua_getfield(L, -1, "Mesh");
        if (lua_type(L, -1) == LUA_TTABLE) {
            lua_getfield(L, -1, key);
        }
        else {
            lua_pushnil(L);
        }
    }

    return 1;
}

static int modules_mesh_mesh_meta_newindex(lua_State* L) {
    mesh_mesh_t* mesh = luaL_checkmeshmesh(L, 1);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "indices") == 0) {
        size_t table_size = lua_rawlen(L, 3);

        if (table_size % 3 != 0) {
            luaL_error(L, "indices length must be a multiple of 3");
        }

        // TODO: Malloc before free? Realloc?
        free(mesh->indices);
        mesh->indices = (uint32_t*)malloc(table_size * sizeof(uint32_t));
        mesh->triangle_count =  table_size / 3;

        for (int i = 0; i < table_size; i++) {
            int index = i + 1;
            lua_pushinteger(L, index);
            lua_gettable(L, 3);

            mesh->indices[i] = (uint32_t)luaL_checknumber(L, -1) - 1;

            lua_pop(L, 1);
        }

        lua_settop(L, 0);

    }
    else if (strcmp(key, "vertices") == 0) {
        size_t table_size = lua_rawlen(L, 3);

        free(mesh->vertices);
        mesh->vertices = (mfloat_t*)malloc(table_size * sizeof(mfloat_t));
        mesh->vertex_count = table_size;

        for (int i = 0; i < table_size; i++) {
            int index = i + 1;
            lua_pushinteger(L, index);
            lua_gettable(L, 3);

            mesh->vertices[i] = (mfloat_t)luaL_checknumber(L, -1);

            lua_pop(L, 1);
        }

        lua_settop(L, 0);
    }
    else if (strcmp(key, "normals") == 0) {
        size_t table_size = lua_rawlen(L, 3);

        if (table_size % 3 != 0) {
            luaL_error(L, "normals length must be a multiple of 3");
        }

        // TODO: Malloc before free? Realloc?
        free(mesh->normals);
        mesh->normals = (mfloat_t*)malloc(table_size * sizeof(mfloat_t));
        //mesh->triangle_count =  table_size / 3;

        for (int i = 0; i < table_size; i++) {
            int index = i + 1;
            lua_pushinteger(L, index);
            lua_gettable(L, 3);

            mesh->normals[i] = (mfloat_t)luaL_checknumber(L, -1);

            lua_pop(L, 1);
        }

        lua_settop(L, 0);

    }
    else if (strcmp(key, "colors") == 0) {
        size_t table_size = lua_rawlen(L, 3);

        if (table_size != mesh->triangle_count) {
            luaL_error(L, "Incorrect color count. Expected: %i, got: %i", mesh->triangle_count, table_size);
            return 0;
        }

        free(mesh->colors);
        mesh->colors = malloc(table_size * sizeof(color_t));

        for (int i = 0; i < table_size; i++) {
            int index = i + 1;
            lua_pushinteger(L, index);
            lua_gettable(L, 3);

            mesh->colors[i] = (mfloat_t)luaL_checknumber(L, -1);

            lua_pop(L, 1);
        }

        lua_settop(L, 0);
    }
    else {
        luaL_error(L, "attempt to index a mesh value");
    }

    return 0;
}

static const struct luaL_Reg modules_mesh_mesh_functions[] = {
    {"new", modules_mesh_mesh_new},
    {NULL, NULL}
};

static int autocomplete_stub(lua_State* L) { return 0; }

static const struct luaL_Reg modules_mesh_mesh_meta_functions[] = {
    {"__index", modules_mesh_mesh_meta_index},
    {"__newindex", modules_mesh_mesh_meta_newindex},
    {"__gc", modules_mesh_mesh_free},
    {"indices", autocomplete_stub},
    {"vertices", autocomplete_stub},
    {NULL, NULL}
};

static mesh_renderer_t* luaL_checkmeshrenderer(lua_State* L, int index) {
    mesh_renderer_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    handle = (mesh_renderer_t**)luaL_checkudata(L, index, "mesh_renderer");

    if (!handle) {
        luaL_typeerror(L, index, "mesh_renderer");
    }

    return *handle;
}

static int lua_newmeshrenderer(lua_State* L) {
    texture_t* render_texture = luaL_opttexture(L, 2, graphics_render_texture_get());
    mesh_renderer_t** handle = (mesh_renderer_t**)lua_newuserdata(L, sizeof(mesh_renderer_t*));
    *handle = mesh_renderer_new(render_texture);
    luaL_setmetatable(L, "mesh_renderer");

    return 1;
}

static int modules_mesh_renderer_free(lua_State* L) {
    mesh_renderer_t** handle = lua_touserdata(L, 1);
    mesh_renderer_free(*handle);
    *handle = NULL;

    return 0;
}

/**
 * @type Renderer
 */

/**
 * Creates a mesh renderer object.
 * @function Renderer:new
 * @tparam ?texture.texture render_texture Optional texture to render to. If none given, will default to global render texture.
 * @treturn Renderer
 */
static int modules_mesh_renderer_new(lua_State* L) {
    return lua_newmeshrenderer(L);
}

/**
 * Clears color buffer.
 * @function Renderer:clear
 * @tparam ?integer color Color to fill buffer with.
 */
static int modules_mesh_renderer_clear(lua_State* L) {
    mesh_renderer_t* renderer = luaL_checkmeshrenderer(L, 1);
    color_t color = (color_t)luaL_optnumber(L, 2, 0);
    //mesh_renderer_clear_color(renderer, color);

    // TODO: Fix the below
    graphics_texture_clear(renderer->render_texture, color);

    return 0;
}

/**
 * Renders given mesh and matrix.
 * @function Renderer:render
 * @tparam Mesh
 * @tparam matrix4
 */
static int modules_mesh_renderer_render(lua_State* L) {
    mesh_renderer_t* renderer = luaL_checkmeshrenderer(L, 1);
    mesh_mesh_t* mesh = luaL_checkmeshmesh(L, 2);
    mfloat_t* matrix = luaL_checkmatrix4(L, 3);

    mesh_renderer_render(
        renderer,
        mesh,
        matrix
    );

    return 0;
}

static int modules_mesh_renderer_camera(lua_State* L) {
    mesh_renderer_t* renderer = luaL_checkmeshrenderer(L, 1);
    mfloat_t* view = luaL_checkmatrix4(L, 2);
    mfloat_t* projection = luaL_checkmatrix4(L, 3);

    mesh_renderer_camera(renderer, view, projection);

    return 0;
}

static int modules_mesh_renderer_meta_index(lua_State* L) {
    luaL_checkmeshrenderer(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_settop(L, 0);

    luaL_requiref(L, "mesh", NULL, false);
    lua_getfield(L, -1, "Renderer");
    if (lua_type(L, -1) == LUA_TTABLE) {
        lua_getfield(L, -1, key);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static const struct luaL_Reg modules_mesh_renderer_functions[] = {
    {"new", modules_mesh_renderer_new},
    {"clear", modules_mesh_renderer_clear},
    {"render", modules_mesh_renderer_render},
    {"camera", modules_mesh_renderer_camera},
    {NULL, NULL}
};

static const struct luaL_Reg modules_mesh_renderer_meta_functions[] = {
    {"__index", modules_mesh_renderer_meta_index},
    {"__gc", modules_mesh_renderer_free},
    {NULL, NULL}
};

int luaopen_mesh(lua_State* L) {
    lua_newtable(L);

    lua_pushstring(L, "Mesh");
    luaL_newlib(L, modules_mesh_mesh_functions);
    lua_settable(L, -3);

    luaL_newmetatable(L, "mesh_mesh");
    luaL_setfuncs(L, modules_mesh_mesh_meta_functions, 0);
    lua_pop(L, 1);

    lua_pushstring(L, "Renderer");
    luaL_newlib(L, modules_mesh_renderer_functions);
    lua_settable(L, -3);

    luaL_newmetatable(L, "mesh_renderer");
    luaL_setfuncs(L, modules_mesh_renderer_meta_functions, 0);
    lua_pop(L, 1);

    return 1;
}
