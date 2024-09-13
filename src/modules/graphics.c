/**
 * Module for drawing to render texture.
 * @module graphics
 */
#include <stdlib.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "graphics.h"
#include "texture.h"

#include "../configuration.h"
#include "../assets.h"
#include "../graphics.h"
#include "../platform.h"

/**
 * Draw a pixel at given position and color.
 * @function set_pixel
 * @tparam integer x Pixel x-coordinate
 * @tparam integer y Pixel y-coordinate
 * @tparam integer color Pixel color
 */
static int modules_graphics_pixel_set(lua_State* L) {
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int color = (int)luaL_checknumber(L, 3);

    lua_pop(L, -1);

    texture_t* render_texture = graphics_render_texture_get();
    graphics_texture_pixel_set(render_texture, x, y, color);

    return 0;
}

/**
 * Blit given texture to render texture.
 * @function blit
 * @tparam texture.texture texture Source texture to blit
 * @tparam integer dx Destination x-offset
 * @tparam integer dy Destination y-offset
 */

/**
 * Blit given texture to render texture.
 * @function blit
 * @tparam texture.texture texture Source texture to blit
 * @tparam integer sx Source x-offset
 * @tparam integer sy Source y-offset
 * @tparam integer sw Source width
 * @tparam integer sh Source height
 * @tparam integer dx Destination x-offset
 * @tparam integer dy Destination y-offset
 * @tparam integer dw Destination width
 * @tparam integer dh Destination height
 */
static int modules_graphics_blit(lua_State* L) {
    int arg_count = lua_gettop(L);

    texture_t* texture = luaL_checktexture(L, 1);
    int sx = 0;
    int sy = 0;
    int sw = texture->width;
    int sh = texture->height;
    int dx = 0;
    int dy = 0;
    int dw = config->resolution.width;
    int dh = config->resolution.height;

    if (arg_count == 3) {
        dx = (int)luaL_checknumber(L, 2);
        dy = (int)luaL_checknumber(L, 3);
        dw = sw;
        dh = sh;
    }
    else {
        sx = (int)luaL_checknumber(L, 2);
        sy = (int)luaL_checknumber(L, 3);
        sw = (int)luaL_checknumber(L, 4);
        sh = (int)luaL_checknumber(L, 5);
        dx = (int)luaL_checknumber(L, 6);
        dy = (int)luaL_checknumber(L, 7);
        dw = (int)luaL_checknumber(L, 8);
        dh = (int)luaL_checknumber(L, 9);
    }

    lua_pop(L, -1);

    rect_t source_rect = {sx, sy, sw, sh};
    rect_t dest_rect = {dx, dy, dw, dh};

    graphics_blit(texture, NULL, &source_rect, &dest_rect, NULL);

    return 0;
}

/**
 * Sets clipping rectangle which defines drawable area.
 * @function set_clipping_rectangle
 * @tparam integer x Rect top left x-coordinate
 * @tparam integer y Rect top left y-coordinate
 * @tparam integer width Rect width
 * @tparam integer height Rect height
 */
static int modules_graphics_clipping_rectangle_set(lua_State* L) {
    int arg_count = lua_gettop(L);

    if (arg_count == 0) {
        graphics_clipping_rectangle_set(NULL);
        return 0;
    }

    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int width = (int)luaL_checknumber(L, 3);
    int height = (int)luaL_checknumber(L, 4);

    lua_pop(L, -1);

    rect_t clip_rect = {x, y, width, height};

    graphics_clipping_rectangle_set(&clip_rect);

    return 0;
}

/**
 * Gets render texture.
 * @function get_render_texture
 * @treturn texture.texture Render texture userdata.
 */
static int modules_graphics_render_texture_get(lua_State* L) {
    lua_pushtexture(L, graphics_render_texture_get());
    return 1;
}

/**
 * Set color for draw palette.
 * @function set_palette_color
 * @tparam integer index Palette index to change.
 * @tparam integer color New color to set.
 */
static int modules_graphics_draw_palette_color_set(lua_State* L) {
    int index = (int)luaL_checknumber(L, 1);
    int color = (int)luaL_checknumber(L, 2);

    color_t* palette = graphics_draw_palette_get();
    palette[index] = color;

    return 0;
}

/**
 * Sets transparent color.
 * @function set_transparent_color
 * @tparam integer color Color set set as transparent.
 */
static int modules_graphics_transparent_color_set(lua_State* L) {
    int color = luaL_optinteger(L, 1, -1);
    graphics_transparent_color_set(color);

    lua_pop(L, -1);

    return 0;
}

/**
 * Set color for graphics palette.
 * @function set_global_palette_color
 * @tparam integer index Index of color to set
 * @tparam integer r Red value. [0, 255]
 * @tparam integer g Green value. [0, 255]
 * @tparam integer b Blue value. [0, 255]
 */
static int modules_graphics_palette_color_set(lua_State* L) {
    int index = (int)lua_tonumber(L, -4);
    int r = (int)lua_tonumber(L, -3) & 0xFF;
    int g = (int)lua_tonumber(L, -2) & 0xFF;
    int b = (int)lua_tonumber(L, -1) & 0xFF;
    int a = 0xFF;

    lua_pop(L, -1);

    uint32_t color = a << 24 | b << 16 | g << 8 | r;

    uint32_t* palette = NULL;
    palette = graphics_palette_get();
    palette[index] = color;

    return 0;
}

/**
 * Set render buffer resolution.
 * @function set_resolution
 * @tparam integer width Resolution width
 * @tparam integer height Resolution height
 */
static int modules_graphics_resolution_set(lua_State* L) {
    int width = (int)luaL_checknumber(L, 1);
    int height = (int)luaL_checknumber(L, 2);

    lua_pop(L, -1);

    config->resolution.width = width;
    config->resolution.height = height;

    graphics_resolution_set(width, height);
    platform_display_resolution_set(width, height);

    return 0;
}

static const struct luaL_Reg modules_graphics_functions[] = {
    {"set_pixel", modules_graphics_pixel_set},
    {"blit", modules_graphics_blit},
    {"set_clipping_rectangle", modules_graphics_clipping_rectangle_set},
    {"get_render_texture", modules_graphics_render_texture_get},
    {"set_palette_color", modules_graphics_draw_palette_color_set},
    {"set_transparent_color", modules_graphics_transparent_color_set},
    {"set_global_palette_color", modules_graphics_palette_color_set},
    {"set_resolution", modules_graphics_resolution_set},
    {NULL, NULL}
};

int luaopen_graphics(lua_State* L) {
    luaL_newlib(L, modules_graphics_functions);
    return 1;
}
