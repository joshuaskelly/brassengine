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

#include "../renderers/raycaster.h"

raycaster_map_t* luaL_checkraycastermap(lua_State* L, int index) {
    raycaster_map_t* map = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    map = (raycaster_map_t*)luaL_checkudata(L, index, "raycaster_map");

    if (!map) {
        luaL_typeerror(L, index, "raycaster_map");
    }

    return map;
}

static int modules_raycaster_map_meta_index(lua_State* L) {
    raycaster_map_t* map = luaL_checkraycastermap(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_pop(L, -1);

    if (strncmp(key, "walls", 5) == 0) {
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
        texture_t* texture = *luaL_checktexture(L, 3);
        map->walls = texture;
    }
    else {
        luaL_error(L, "attempt to index a raycaster_mapmap value");
    }

    lua_pop(L, -1);

    return 0;
}

static const struct luaL_Reg raycaster_map_meta_functions[] = {
    {"__index", modules_raycaster_map_meta_index},
    {"__newindex", modules_raycaster_map_meta_newindex},
    {NULL, NULL}
};

static int modules_raycaster_map_new(lua_State* L) {
    raycaster_map_t* map = (raycaster_map_t*)lua_newuserdata(L, sizeof(raycaster_map_t));
    map->walls = NULL;
    luaL_setmetatable(L, "raycaster_map");

    return 1;
}

/**
 * Renders given map data.
 * @function render
 * @param px Camera position x-coordinate
 * @param py Camera position y-coordinate
 * @param dx Camera forward x-coordinate
 * @param dx Camera forward y-coordinate
 * @param fov Camera field of view in degrees
 * @param map Map as texture
 * @param texture Render texture
 */

/**
 * Renders given map data.
 * @function render
 * @param px Camera position x-coordinate
 * @param py Camera position y-coordinate
 * @param dx Camera forward x-coordinate
 * @param dx Camera forward y-coordinate
 * @param fov Camera field of view in degrees
 * @param map Map as texture
 * @param texture Render texture
 * @param rx Render texture x-offset
 * @param ry Render texture y-offset
 * @param rw Render texture width
 * @param rh Render texture height
 */
static int module_raycaster_render(lua_State* L) {
    float px = luaL_checknumber(L, 1);
    float py = luaL_checknumber(L, 2);
    float dx = luaL_checknumber(L, 3);
    float dy = luaL_checknumber(L, 4);
    float fov = luaL_checknumber(L, 5);
    raycaster_map_t* map = luaL_checkraycastermap(L, 6);
    texture_t** render_texture = luaL_checktexture(L, 7);
    float rx = luaL_optnumber(L, 8, 0);
    float ry = luaL_optnumber(L, 9, 0);
    float rw = luaL_optnumber(L, 10, (*render_texture)->width);
    float rh = luaL_optnumber(L, 11, (*render_texture)->height);

    lua_pop(L, 1);

    mfloat_t position[VEC2_SIZE] = {px, py};
    mfloat_t direction[VEC2_SIZE] = {dx, dy};
    rect_t rect = {rx, ry, rw, rh};

    raycaster_render(
        position,
        direction,
        fov,
        map,
        *render_texture,
        &rect
    );

    return 0;
}

/**
 * Sets texture in texture palette.
 * @function palette
 * @param index Texture palette index
 * @param texture Texture for given index
 */
static int module_raycaster_palette(lua_State* L) {
    int i = (int)luaL_checknumber(L, 1);
    texture_t** texture = luaL_checktexture(L, 2);

    lua_pop(L, 1);

    raycaster_set_texture(i, *texture);

    return 0;
}

/**
 * Sets texture to use as a color lookup table for shading.
 * @function shade_table
 * @param texture Texture to use as lookup table.
 */
static int module_raycaster_shade_table_set(lua_State* L) {
    texture_t** texture = luaL_checktexture(L, 1);

    lua_pop(L, 1);

    raycaster_shade_table_set(*texture);

    return 0;
}

/**
 * Sets far distance for fog.
 * @function fog_distance
 * @param distance Distance where fog effect is greatest.
 */
static int module_raycaster_fog_distance_set(lua_State* L) {
    float distance = luaL_checknumber(L, 1);

    raycaster_fog_distance_set(distance);

    return 0;
}

static const struct luaL_Reg module_functions[] = {
    {"render", module_raycaster_render},
    {"palette", module_raycaster_palette},
    {"shade_table", module_raycaster_shade_table_set},
    {"fog_distance", module_raycaster_fog_distance_set},
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
