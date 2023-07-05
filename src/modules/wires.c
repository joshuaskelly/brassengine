#include <stdlib.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "matrix4.h"
#include "wires.h"

#include "../renderers/wires.h"

wires_renderer_t* luaL_checkwiresrenderer(lua_State* L, int index) {
    wires_renderer_t* renderer = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    renderer = (wires_renderer_t*)luaL_checkudata(L, index, "wires_renderer");

    if (!renderer) {
        luaL_typeerror(L, index, "wires_renderer");
    }

    return renderer;
}

int lua_newwiresrenderer(lua_State* L) {
    wires_renderer_t* renderer = (wires_renderer_t*)lua_newuserdata(L, sizeof(wires_renderer_t));
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

    luaL_newmetatable(L, "wires_renderer");
    luaL_setfuncs(L, wires_renderer_meta_functions, 0);
    lua_pop(L, 1);

    return 1;
}
