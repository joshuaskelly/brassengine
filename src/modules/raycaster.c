/**
 * Module for raycaster renderer.
 * @module raycaster
 */
#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "raycaster.h"
#include "texture.h"

#include "../renderers/raycaster.h"

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
    texture_t** texture = luaL_checktexture(L, 6);
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
        *texture,
        *render_texture,
        &rect);
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
    {NULL, NULL}
};

int luaopen_raycaster(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
