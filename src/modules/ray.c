/**
 * Module for raycaster renderer.
 * @module ray
 */
#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "ray.h"
#include "texture.h"
#include "vector2.h"

#include "../renderers/raycaster.h"

static raycaster_renderer_t* luaL_checkrayrenderer(lua_State* L, int index) {
    raycaster_renderer_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    handle = (raycaster_renderer_t**)luaL_checkudata(L, index, "ray_renderer");

    if (!handle) {
        luaL_typeerror(L, index, "ray_renderer");
    }

    return *handle;
}

static int lua_newrayrenderer(lua_State* L) {
    raycaster_renderer_t** handle = (raycaster_renderer_t**)lua_newuserdata(L, sizeof(raycaster_renderer_t*));
    *handle = raycaster_renderer_new(NULL);
    luaL_setmetatable(L, "ray_renderer");

    return 1;
}

static int module_ray_renderer_new(lua_State* L) {
    return lua_newrayrenderer(L);
}

static int module_ray_renderer_clear(lua_State* L) {
    raycaster_renderer_t* renderer = luaL_checkrayrenderer(L, 1);

    const char* name = luaL_optstring(L, 2, "all");

    if (strcmp(name, "all") == 0) {
        raycaster_renderer_clear_color(renderer, 0);
        raycaster_renderer_clear_depth(renderer, 1.0f);
    }
    else if (strcmp(name, "depth") == 0) {
        raycaster_renderer_clear_depth(renderer, 1.0f);
    }
    else if (strcmp(name, "color") == 0) {
        raycaster_renderer_clear_color(renderer, 0);
    }
    else {
        luaL_argerror(L, 2, lua_pushfstring(L, "invalid option '%s'", name));
    }

    return 0;
}

static int module_ray_renderer_render(lua_State* L) {
    raycaster_renderer_t* renderer = luaL_checkrayrenderer(L, 1);

    raycaster_map_t** handle = NULL;
    luaL_checktype(L, 2, LUA_TUSERDATA);
    handle = (raycaster_map_t**)luaL_testudata(L, 2, "raycaster_map");

    if (handle) {
        raycaster_map_t* map = *handle;
        raycaster_renderer_render_map(renderer, map);
    }
    else {
        texture_t* sprite = luaL_checktexture(L, 2);
        mfloat_t* position = luaL_checkvector2(L, 3);
        raycaster_renderer_render_sprite(renderer, sprite, position);
    }

    return 0;
}

static int module_ray_renderer_camera(lua_State* L) {
    raycaster_renderer_t* renderer = luaL_checkrayrenderer(L, 1);
    mfloat_t* position = luaL_checkvector2(L, 2);
    mfloat_t* direction = luaL_checkvector2(L, 3);
    float fov = luaL_checknumber(L, 4);

    raycaster_renderer_camera(renderer, position, direction, fov);

    return 0;
}

static int module_ray_renderer_feature(lua_State* L) {
    raycaster_renderer_t* renderer = luaL_checkrayrenderer(L, 1);
    const char* key = luaL_checkstring(L, 2);
    bool is_setter = lua_gettop(L) > 2;

    if (strcmp(key, "fog") == 0) {
        if (is_setter) {
            float fog_distance = luaL_checknumber(L, 3);
            renderer->fog_distance = fog_distance;

            return 0;
        }

        lua_pushnumber(L, renderer->fog_distance);

        return 1;
    }
    else if (strcmp(key, "shadetable") == 0) {
        if (is_setter) {
            texture_t* shade_table = luaL_checktexture(L, 3);
            renderer->shade_table = shade_table;

            return 0;
        }

        if (renderer->shade_table) {
            lua_pushtexture(L, renderer->shade_table);
        }
        else {
            lua_pushnil(L);
        }

        return 1;
    }
    else {
        luaL_argerror(L, 2, lua_pushfstring(L, "invalid feature '%s'", key));
    }

    return 0;
}

static int module_ray_renderer_meta_index(lua_State* L) {
    luaL_checkrayrenderer(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_settop(L, 0);

    luaL_requiref(L, "ray", NULL, false);
    lua_getfield(L, -1, "Renderer");
    if (lua_type(L, -1) == LUA_TTABLE) {
        lua_getfield(L, -1, key);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static int module_ray_map_new(lua_State* L) {
    raycaster_map_t** handle = (raycaster_map_t**)lua_newuserdata(L, sizeof(raycaster_map_t*));
    *handle = raycaster_map_new();
    luaL_setmetatable(L, "raycaster_map");

    return 1;
}

static int module_ray_map_meta_index(lua_State* L) {
    luaL_checkrayrenderer(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_settop(L, 0);

    luaL_requiref(L, "ray", NULL, false);
    lua_getfield(L, -1, "Map");
    if (lua_type(L, -1) == LUA_TTABLE) {
        lua_getfield(L, -1, key);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static const struct luaL_Reg ray_map_functions[] = {
    {"new", module_ray_map_new},
    {NULL, NULL}
};

static const struct luaL_Reg ray_map_meta_functions[] = {
    {"__index", module_ray_map_meta_index},
    {NULL, NULL}
};

static const struct luaL_Reg ray_renderer_functions[] = {
    {"new", module_ray_renderer_new},
    {"clear", module_ray_renderer_clear},
    {"render", module_ray_renderer_render},
    {"camera", module_ray_renderer_camera},
    {"feature", module_ray_renderer_feature},
    {NULL, NULL}
};

static const struct luaL_Reg ray_renderer_meta_functions[] = {
    {"__index", module_ray_renderer_meta_index},
    {NULL, NULL}
};

int luaopen_ray(lua_State* L) {
    lua_newtable(L);

    lua_pushstring(L, "Renderer");
    luaL_newlib(L, ray_renderer_functions);
    lua_settable(L, -3);

    luaL_newmetatable(L, "ray_renderer");
    luaL_setfuncs(L, ray_renderer_meta_functions, 0);
    lua_pop(L, 1);

    lua_pushstring(L, "Map");
    luaL_newlib(L, ray_map_functions);
    lua_settable(L, -3);

    luaL_newmetatable(L, "ray_map");
    luaL_setfuncs(L, ray_map_meta_functions, 0);
    lua_pop(L, 1);

    return 1;
}