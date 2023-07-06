#include <stdlib.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

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
        line_buffer->indices[i] = indices[i];
    }

    for (int i = 0; i < color_count; i++) {
        line_buffer->colors[i] = colors[i];
    }

    for (int i = 0; i < vertex_count; i++) {
        line_buffer->vertices[i] = vertices[i];
    }

    return 1;
}

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

static int module_wires_renderer_new(lua_State* L) {
    lua_newwiresrenderer(L);

    return 1;
}

static int module_wires_renderer_start(lua_State* L) {
    wires_renderer_t* renderer = luaL_checkwiresrenderer(L, 1);

    wires_renderer_start(renderer);

    return 0;
}

static int module_wires_renderer_stop(lua_State* L) {
    wires_renderer_t* renderer = luaL_checkwiresrenderer(L, 1);

    wires_renderer_stop(renderer);

    return 0;
}

static int module_wires_renderer_render(lua_State* L) {
    wires_renderer_t* renderer = luaL_checkwiresrenderer(L, 1);
    mfloat_t* model_matrix = luaL_checkmatrix4(L, 2);
    wires_line_buffer_t* lines = luaL_checkwireslinebuffer(L, 3);

    wires_renderer_render(renderer, model_matrix, lines);

    return 0;
}

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
    wires_renderer_t* renderer = luaL_checkwiresrenderer(L, 1);
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
