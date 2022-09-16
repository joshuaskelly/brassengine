#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "graphics.h"

#include "../graphics.h"

/**
 * Draw a pixel at given position and color.
 *
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @param color Pixel color
 */
int bindings_graphics_set_pixel(lua_State* L) {
    int x = (int)lua_tonumber(L, -3);
    int y = (int)lua_tonumber(L, -2);
    int color = (int)lua_tonumber(L, -1);

    lua_pop(L, -1);

    texture_t* render_texture = graphics_get_render_texture();
    graphics_texture_set_pixel(render_texture, x, y, color);

    return 0;
}

int bindings_graphics_set_clipping_rectangle(lua_State* L) {
    int x = (int)lua_tonumber(L, -4);
    int y = (int)lua_tonumber(L, -3);
    int width = (int)lua_tonumber(L, -2);
    int height = (int)lua_tonumber(L, -1);

    lua_pop(L, -1);

    graphics_set_clipping_rectangle(x, y, width, height);

    return 0;
}

int bindings_graphics_get_render_texture(lua_State* L) {
    lua_pushlightuserdata(L, graphics_get_render_texture());
    return 1;
}

static const struct luaL_Reg module_functions[] = {
    {"set_pixel", bindings_graphics_set_pixel},
    {"set_clipping_rectangle", bindings_graphics_set_clipping_rectangle},
    {"get_render_texture", bindings_graphics_get_render_texture},
    {NULL, NULL}
};

int open_graphics_module(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
