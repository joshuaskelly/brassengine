#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "texture.h"

#include "../graphics.h"

/**
 * Draw a pixel at given position and color.
 *
 * @param texture Texture userdata
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @param color Pixel color
 */
int bindings_texture_set_pixel(lua_State* L) {
    texture_t* texture = lua_touserdata(L, 1);
    int x = (int)lua_tonumber(L, 2);
    int y = (int)lua_tonumber(L, 3);
    int color = (int)lua_tonumber(L, 4);

    lua_pop(L, -1);

    graphics_texture_set_pixel(texture, x, y, color);

    return 0;
}

static const struct luaL_Reg module_functions[] = {
    {"set_pixel", bindings_texture_set_pixel},
    {NULL, NULL}
};

int open_texture_module(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
