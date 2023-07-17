/**
 * Module for wireframe renderer
 *
 * @usage
 * wires = require('wires')
 *
 * indices = {0, 1}
 * colors = {1}
 * vertices = {0, 0, 10, 10}
 *
 * lines = wires.Lines:new(indices, colors, vertices)
 * renderer = wires.Renderer:new()
 *
 * renderer:start()
 * renderer:clear()
 * renderer:camera(0.01, 100, math.rad(90), view_matrix)
 * renderer:render(lines)
 * renderer:stop()
 *
 * @module wires
 */

#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "int_array.h"
#include "float_array.h"
#include "matrix4.h"
#include "wires.h"

#include "../renderers/wires.h"

static wires_line_buffer_t* luaL_checkwireslinebuffer(lua_State* L, int index) {
    wires_line_buffer_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    handle = (wires_line_buffer_t**)luaL_checkudata(L, index, "wires_lines");

    return *handle;
}

static int lua_newwireslinebuffer(lua_State* L, int line_count, int vertex_count) {
    wires_line_buffer_t** handle = (wires_line_buffer_t**)lua_newuserdata(L, sizeof(wires_line_buffer_t*));
    *handle = wires_line_buffer_new(line_count, vertex_count);
    luaL_setmetatable(L, "wires_lines");

    return 1;
}

/**
 * Submodule Lines
 * @section Lines
 */

/**
 * Creates a wires_lines object.
 * @function Lines.new
 * @param indices Array of integer index pairs.
 * @param colors Array of integer colors.
 * @param vertices Array of interlaced float x, y, z coordinates.
 * @return @{wires_lines}
 */
static int module_wires_line_buffer_new(lua_State* L) {
    // Indices
    if (!lua_istable(L, 1)) {
        luaL_typeerror(L, 1, "table");
    }

    size_t index_count = lua_rawlen(L, 1);
    int indices[index_count];

    for (int i = 0; i < index_count; i++) {
        int index = i + 1;
        lua_pushinteger(L, index);
        lua_gettable(L, 1);

        if (lua_type(L, -1) == LUA_TNIL) break;

        indices[i] = (int)luaL_checknumber(L, -1);

        lua_pop(L, 1);
    }

    // Colors
    if (!lua_istable(L, 2)) {
        luaL_typeerror(L, 2, "table");
    }

    size_t color_count = lua_rawlen(L, 2);
    int colors[color_count];

    if (index_count != color_count * 2) {
        luaL_error(L, "Incorrect number of indices");
    }

    for (int i = 0; i < color_count; i++) {
        int index = i + 1;
        lua_pushinteger(L, index);
        lua_gettable(L, 2);

        if (lua_type(L, -1) == LUA_TNIL) break;

        colors[i] = (int)luaL_checknumber(L, -1);

        lua_pop(L, 1);
    }

    // Vertices
    if (!lua_istable(L, 3)) {
        luaL_typeerror(L, 3, "table");
    }

    size_t vertex_count = lua_rawlen(L, 3);
    mfloat_t vertices[vertex_count];

    for (int i = 0; i < vertex_count; i++) {
        int index = i + 1;
        lua_pushinteger(L, index);
        lua_gettable(L, 3);

        if (lua_type(L, -1) == LUA_TNIL) break;

        vertices[i] = (mfloat_t)luaL_checknumber(L, -1);

        lua_pop(L, 1);
    }

    lua_settop(L, 0);

    lua_newwireslinebuffer(L, index_count / 2, vertex_count);

    wires_line_buffer_t* line_buffer = luaL_checkwireslinebuffer(L, 1);

    for (int i = 0; i < index_count; i++) {
        line_buffer->indices->data[i] = indices[i];
    }

    for (int i = 0; i < color_count; i++) {
        line_buffer->colors->data[i] = colors[i];
    }

    for (int i = 0; i < vertex_count; i++) {
        line_buffer->vertices->data[i] = vertices[i];
    }

    return 1;
}

static int modules_wires_line_buffer_meta_index(lua_State* L) {
    wires_line_buffer_t* lines = luaL_checkwireslinebuffer(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_settop(L, 0);

    if (strcmp(key, "indices") == 0) {
        lua_pushintarray(L, lines->indices);
    }
    else if (strcmp(key, "colors") == 0) {
        lua_pushintarray(L, lines->colors);
    }
    else if (strcmp(key, "vertices") == 0) {
        lua_pushfloatarray(L, lines->vertices);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static int modules_wires_line_buffer_meta_gc(lua_State* L) {
    wires_line_buffer_t* lines = luaL_checkwireslinebuffer(L, 1);
    wires_line_buffer_free(lines);

    return 0;
}

/**
 * @type wires_lines
 */

/**
 * Flattened array of index pairs.
 * @field indices @{intarray}
 */

/**
 * Array of color indices.
 * @field colors @{intarray}
 */

/**
 * Flattened array of vertex x, y, z coordinates.
 * @field vertices @{floatarray}
 */

static wires_renderer_t* luaL_checkwiresrenderer(lua_State* L, int index) {
    wires_renderer_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    handle = (wires_renderer_t**)luaL_checkudata(L, index, "wires_renderer");

    if (!handle) {
        luaL_typeerror(L, index, "wires_renderer");
    }

    return *handle;
}

static int lua_newwiresrenderer(lua_State* L) {
    wires_renderer_t** handle = (wires_renderer_t**)lua_newuserdata(L, sizeof(wires_renderer_t*));
    *handle = wires_renderer_new();
    luaL_setmetatable(L, "wires_renderer");

    return 1;
}

/**
 * Submodule Renderer
 * @section Renderer
 */

/**
 * Creates a wires_renderer object.
 * @within Submodule Renderer
 * @function Renderer.new
 * @return @{wires_renderer}
 */
static int module_wires_renderer_new(lua_State* L) {
    lua_newwiresrenderer(L);

    return 1;
}

/**
 * @type wires_renderer
 */

/**
 * Start rendering.
 * @function start
 */
static int module_wires_renderer_start(lua_State* L) {
    wires_renderer_t* renderer = luaL_checkwiresrenderer(L, 1);

    wires_renderer_start(renderer);

    return 0;
}

/**
 * Stop rendering.
 * @function stop
 */
static int module_wires_renderer_stop(lua_State* L) {
    wires_renderer_t* renderer = luaL_checkwiresrenderer(L, 1);

    wires_renderer_stop(renderer);

    return 0;
}

/**
 * Clears renderer's internal buffers.
 * @function clear
 * @param option Which buffer to clear. 'color' to clear the color buffer. 'depth' to clear the depth buffer. 'all' to clear both.
 */
static int module_wires_renderer_clear(lua_State* L) {
    wires_renderer_t* renderer = luaL_checkwiresrenderer(L, 1);

    const char* name = luaL_optstring(L, 2, "all");

    if (strcmp(name, "all") == 0) {
        wires_renderer_clear_color(renderer, 0);
        wires_renderer_clear_depth(renderer, 1.0f);
    }
    else if (strcmp(name, "depth") == 0) {
        wires_renderer_clear_depth(renderer, 1.0f);
    }
    else if (strcmp(name, "color") == 0) {
        wires_renderer_clear_color(renderer, 0);
    }
    else {
        luaL_argerror(L, 2, lua_pushfstring(L, "invalid option '%s'", name));
    }

    return 0;
}

/**
 * Renders a wires_line object.
 * @function render
 * @param lines @{wires_lines}
 */
static int module_wires_renderer_render(lua_State* L) {
    wires_renderer_t* renderer = luaL_checkwiresrenderer(L, 1);
    mfloat_t* model_matrix = luaL_checkmatrix4(L, 2);
    wires_line_buffer_t* lines = luaL_checkwireslinebuffer(L, 3);

    wires_renderer_render(renderer, model_matrix, lines);

    return 0;
}

/**
 * Set renderer's camera data.
 * @function camera
 * @param near Near clipping plane distance.
 * @param far Far clipping plane distance.
 * @param fov Field of view in radians.
 * @param view_matrix @{matrix4} View matrix.
 */
static int module_wires_renderer_camera(lua_State* L) {
    wires_renderer_t* renderer = luaL_checkwiresrenderer(L, 1);
    float near = luaL_checknumber(L, 2);
    float far = luaL_checknumber(L, 3);
    float fov = luaL_checknumber(L, 4);
    mfloat_t* view_matrix = luaL_checkmatrix4(L, 5);

    wires_renderer_camera(
        renderer,
        near,
        far,
        fov,
        view_matrix
    );

    return 0;
}

static int module_wires_renderer_meta_index(lua_State* L) {
    luaL_checkwiresrenderer(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_settop(L, 0);

    luaL_requiref(L, "wires", NULL, false);
    lua_getfield(L, -1, "Renderer");
    if (lua_type(L, -1) == LUA_TTABLE) {
        lua_getfield(L, -1, key);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static const struct luaL_Reg wires_line_buffer_meta_functions[] = {
    {"__index", modules_wires_line_buffer_meta_index},
    {"__gc", modules_wires_line_buffer_meta_gc},
    {NULL, NULL}
};

static const struct luaL_Reg wires_line_buffer_functions[] = {
    {"new", module_wires_line_buffer_new},
    {NULL, NULL}
};

static const struct luaL_Reg wires_renderer_meta_functions[] = {
    {"__index", module_wires_renderer_meta_index},
    {NULL, NULL}
};

static const struct luaL_Reg wires_renderer_functions[] = {
    {"new", module_wires_renderer_new},
    {"start", module_wires_renderer_start},
    {"stop", module_wires_renderer_stop},
    {"clear", module_wires_renderer_clear},
    {"render", module_wires_renderer_render},
    {"camera", module_wires_renderer_camera},
    {NULL, NULL}
};

int luaopen_wires(lua_State* L) {
    lua_newtable(L);

    lua_pushstring(L, "Renderer");
    luaL_newlib(L, wires_renderer_functions);
    lua_settable(L, -3);

    lua_pushstring(L, "Lines");
    luaL_newlib(L, wires_line_buffer_functions);
    lua_settable(L, -3);

    luaL_newmetatable(L, "wires_renderer");
    luaL_setfuncs(L, wires_renderer_meta_functions, 0);
    lua_pop(L, 1);

    luaL_newmetatable(L, "wires_lines");
    luaL_setfuncs(L, wires_line_buffer_meta_functions, 0);
    lua_pop(L, 1);

    return 1;
}
