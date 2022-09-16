#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "texture.h"

#include "../graphics.h"

int bindings_texture_clear(lua_State* L) {
    texture_t** texture = lua_touserdata(L, 1);
    int color = (int)lua_tonumber(L, 2);

    lua_pop(L, -1);

    graphics_texture_clear(*texture, color);

    return 0;
}

/**
 * Draw a pixel at given position and color.
 *
 * @param texture Texture userdata
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @param color Pixel color
 */
int bindings_texture_set_pixel(lua_State* L) {
    texture_t** texture = lua_touserdata(L, 1);
    int x = (int)lua_tonumber(L, 2);
    int y = (int)lua_tonumber(L, 3);
    int color = (int)lua_tonumber(L, 4);

    lua_pop(L, -1);

    graphics_texture_set_pixel(*texture, x, y, color);

    return 0;
}

/**
 * Gets pixel at given position.
 *
 * @param texture Texture userdata
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 */
int bindings_texture_get_pixel(lua_State* L) {
    texture_t** texture = lua_touserdata(L, 1);
    int x = (int)lua_tonumber(L, 2);
    int y = (int)lua_tonumber(L, 3);

    lua_pop(L, -1);

    color_t color = graphics_texture_get_pixel(*texture, x, y);
    lua_pushinteger(L, color);

    return 1;
}

int bindings_texture_blit(lua_State* L) {
    texture_t** source = lua_touserdata(L, 1);
    texture_t** dest = lua_touserdata(L, 2);
    int x = (int)lua_tonumber(L, 3);
    int y = (int)lua_tonumber(L, 4);

    rect_t drect = {x, y, (*source)->width, (*source)->height};

    graphics_texture_blit(*source, *dest, NULL, &drect);

    return 0;
}

static const struct luaL_Reg module_functions[] = {
    {"clear", bindings_texture_clear},
    {"set_pixel", bindings_texture_set_pixel},
    {"get_pixel", bindings_texture_get_pixel},
    {"blit", bindings_texture_blit},
    {NULL, NULL}
};

int open_texture_module(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
