#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "graphics.h"
#include "texture.h"

#include "../assets.h"
#include "../graphics.h"

/**
 * Draw a pixel at given position and color.
 *
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @param color Pixel color
 */
static int bindings_graphics_set_pixel(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int color = luaL_checkinteger(L, 3);

    lua_pop(L, -1);

    texture_t* render_texture = graphics_get_render_texture();
    graphics_texture_set_pixel(render_texture, x, y, color);

    return 0;
}

/**
 * Blit given texture to render texture
 *
 * @param texture Source texture to blit
 * @param sx Source x-offset
 * @param sy Source y-offset
 * @param sw Source width
 * @param sh Source height
 * @param dx Destination x-offset
 * @param dy Destination y-offset
 * @param dw Destination width
 * @param dh Destination height
 */
static int bindings_graphics_blit(lua_State* L) {
    texture_t** texture = luaL_checktexture(L, 1);
    int sx = luaL_checkinteger(L, 2);
    int sy = luaL_checkinteger(L, 3);
    int sw = luaL_checkinteger(L, 4);
    int sh = luaL_checkinteger(L, 5);
    int dx = luaL_checkinteger(L, 6);
    int dy = luaL_checkinteger(L, 7);
    int dw = luaL_checkinteger(L, 8);
    int dh = luaL_checkinteger(L, 9);

    lua_pop(L, -1);

    rect_t source_rect = {sx, sy, sw, sh};
    rect_t dest_rect = {dx, dy, dw, dh};

    graphics_blit(*texture, &source_rect, &dest_rect);

    return 0;
}

/**
 * Sets clipping rectangle which defines drawable area.
 *
 * @param x Rect top left x-coordinate
 * @param y Rect top left y-coordinate
 * @param width Rect width
 * @param height Rect height
 */
static int bindings_graphics_set_clipping_rectangle(lua_State* L) {
    int x = luaL_checkinteger(L, 1);
    int y = luaL_checkinteger(L, 2);
    int width = luaL_checkinteger(L, 3);
    int height = luaL_checkinteger(L, 4);

    lua_pop(L, -1);

    graphics_set_clipping_rectangle(x, y, width, height);

    return 0;
}

/**
 * Gets render texture.
 *
 * @return Render texture userdata.
 */
static int bindings_graphics_get_render_texture(lua_State* L) {
    texture_t** tp = (texture_t**)lua_newuserdata(L, sizeof(texture_t*));
    *tp = graphics_get_render_texture();
    luaL_setmetatable(L, "texture");
    return 1;
}

/**
 * Set color for draw palette.
 *
 * @param index Palette index to change.
 * @param color New color to set.
 */
static int bindings_graphics_set_draw_palette_color(lua_State* L) {
    int index = luaL_checkinteger(L, 1);
    int color = luaL_checkinteger(L, 2);

    uint32_t* palette = graphics_draw_palette_get();
    palette[index] = color;

    return 0;
}

static int bindings_graphics_set_transparent_color(lua_State* L) {
    int color = luaL_optinteger(L, 1, -1);
    graphics_transparent_color_set(color);

    lua_pop(L, -1);

    return 0;
}

static const struct luaL_Reg module_functions[] = {
    {"set_pixel", bindings_graphics_set_pixel},
    {"blit", bindings_graphics_blit},
    {"set_clipping_rectangle", bindings_graphics_set_clipping_rectangle},
    {"get_render_texture", bindings_graphics_get_render_texture},
    {"set_palette_color", bindings_graphics_set_draw_palette_color},
    {"set_transparent_color", bindings_graphics_set_transparent_color},
    {NULL, NULL}
};

int open_graphics_module(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
