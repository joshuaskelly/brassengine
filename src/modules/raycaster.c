/**
 * Module for raycaster renderer.
 * @module raycaster
 */
#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "raycaster.h"
#include "texture.h"
#include "vector2.h"

#include "../renderers/raycaster.h"

raycaster_map_t* luaL_checkraycastermap(lua_State* L, int index) {
    raycaster_map_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    handle = (raycaster_map_t**)luaL_checkudata(L, index, "raycaster_map");

    return *handle;
}

static int modules_raycaster_map_meta_index(lua_State* L) {
    raycaster_map_t* map = luaL_checkraycastermap(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_pop(L, -1);

    if (strcmp(key, "walls") == 0) {
        lua_pushtexture(L, map->walls);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static int modules_raycaster_map_meta_newindex(lua_State* L) {
    raycaster_map_t* map = luaL_checkraycastermap(L, 1);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "walls") == 0) {
        texture_t* texture = luaL_checktexture(L, 3);
        map->walls = texture;
    }
    else {
        luaL_error(L, "attempt to index a raycaster_mapmap value");
    }

    lua_pop(L, -1);

    return 0;
}

static int modules_raycaster_map_meta_gc(lua_State* L) {
    raycaster_map_t** handle = lua_touserdata(L, 1);
    raycaster_map_free(*handle);
    *handle = NULL;

    return 0;
}

static const struct luaL_Reg raycaster_map_meta_functions[] = {
    {"__index", modules_raycaster_map_meta_index},
    {"__newindex", modules_raycaster_map_meta_newindex},
    {"__gc", modules_raycaster_map_meta_gc},
    {NULL, NULL}
};

/**
 * Create a new raycaster_map object.
 * @function map_new
 * @return raycaster_map
 */
static int modules_raycaster_map_new(lua_State* L) {
    raycaster_map_t** handle = (raycaster_map_t**)lua_newuserdata(L, sizeof(raycaster_map_t*));
    *handle = raycaster_map_new();
    luaL_setmetatable(L, "raycaster_map");

    return 1;
}

/**
 * Map for rendering a raycaster scene.
 * @type raycaster_map
 */

/**
 * Map walls.
 * @field walls A texture userdata representing wall positions and textures.
 */

static const struct luaL_Reg module_functions[] = {
    {"map_new", modules_raycaster_map_new},
    {NULL, NULL}
};

int luaopen_raycaster(lua_State* L) {
    luaL_newlib(L, module_functions);

    luaL_newmetatable(L, "raycaster_map");
    luaL_setfuncs(L, raycaster_map_meta_functions, 0);
    lua_pop(L, 1);

    return 1;
}
