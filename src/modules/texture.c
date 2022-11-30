#include <stdbool.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "texture.h"

#include "../graphics.h"

texture_t** luaL_checktexture(lua_State* L, int index) {
    texture_t** texture = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    texture = (texture_t**)luaL_checkudata(L, index, "texture");

    if (!texture) {
        luaL_typeerror(L, index, "texture");
    }

    return texture;
}

/**
 * Texture garbage collection metamethod.
 *
 * @param L Lua VM
 * @return int
 */
    // TODO: Only free up dynamic textures, not ones acquire from assets
static int texture_gc(lua_State* L) {
    //texture_t** texture = lua_touserdata(L, 1);
    //graphics_texture_free(*texture);

    return 0;
}

static int texture_size(lua_State* L) {
    texture_t** texture = luaL_checktexture(L, 1);

    lua_pushinteger(L, (*texture)->width);
    lua_pushinteger(L, (*texture)->height);

    return 2;
}

static const struct luaL_Reg texture_methods[] = {
    {"size", texture_size},
    {NULL, NULL}
};

/**
 * Create new texture.
 *
 * @param width Texture width
 * @param height Texture height
 * @return Texture userdata
 */
static int bindings_texture_new(lua_State* L) {
    int width = luaL_checkinteger(L, 1);
    int height = luaL_checkinteger(L, 2);

    lua_pop(L, -1);

    texture_t** texture = (texture_t**)lua_newuserdata(L, sizeof(texture_t*));
    *texture = graphics_texture_new(width, height, NULL);
    luaL_setmetatable(L, "texture");

    return 1;
}

/**
 * Copy given texture.
 *
 * @param texture Texture to copy
 * @return Texture userdata
 */
static int bindings_texture_copy(lua_State* L) {
    texture_t** source = luaL_checktexture(L, 1);

    lua_pop(L, -1);

    texture_t** texture = (texture_t**)lua_newuserdata(L, sizeof(texture_t*));
    *texture = graphics_texture_copy(*source);
    luaL_setmetatable(L, "texture");

    return 1;
}

/**
 * Fill entire texture with color.
 *
 * @param texture
 * @param color
 */
static int bindings_texture_clear(lua_State* L) {
    texture_t** texture = luaL_checktexture(L, 1);
    int color = luaL_checkinteger(L, 2);

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
static int bindings_texture_set_pixel(lua_State* L) {
    texture_t** texture = luaL_checktexture(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int color = luaL_checkinteger(L, 4);

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
static int bindings_texture_get_pixel(lua_State* L) {
    texture_t** texture = luaL_checktexture(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);

    lua_pop(L, -1);

    color_t color = graphics_texture_get_pixel(*texture, x, y);
    lua_pushinteger(L, color);

    return 1;
}

/**
 * Copy a portion of one texture to another.
 *
 * @param source_texture Texture to copy from
 * @param destination_texture Texture to copy to
 * @param x
 * @param y
 */
static int bindings_texture_blit(lua_State* L) {
    texture_t** source = luaL_checktexture(L, 1);
    texture_t** dest = luaL_checktexture(L, 2);
    int x = luaL_checkinteger(L, 3);
    int y = luaL_checkinteger(L, 4);

    rect_t drect = {x, y, (*source)->width, (*source)->height};

    graphics_texture_blit(*source, *dest, NULL, &drect);

    return 0;
}

static const struct luaL_Reg module_functions[] = {
    {"new", bindings_texture_new},
    {"copy", bindings_texture_copy},
    {"clear", bindings_texture_clear},
    {"set_pixel", bindings_texture_set_pixel},
    {"get_pixel", bindings_texture_get_pixel},
    {"blit", bindings_texture_blit},
    {NULL, NULL}
};

int open_texture_module(lua_State* L) {
    luaL_newlib(L, module_functions);

    // Push texture userdata metatable
    luaL_newmetatable(L, "texture");
    luaL_newlib(L, texture_methods);
    lua_setfield(L, -2, "__index");

    lua_pushstring(L, "__gc");
    lua_pushcfunction(L, texture_gc);
    lua_settable(L, -3);

    lua_pop(L, 1);

    return 1;
}
