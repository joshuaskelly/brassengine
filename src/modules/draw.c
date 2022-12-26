/**
 * Module for drawing geometric shapes.
 * @module draw
 */

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "draw.h"
#include "texture.h"
#include "../assets.h"
#include "../graphics.h"
#include "../renderers/draw.h"

/**
 * Draw a pixel at given position and color.
 * @function pixel
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @param color Pixel color
 */
static int bindings_draw_pixel(lua_State* L) {
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int color = (int)luaL_checknumber(L, 3);

    lua_pop(L, -1);

    texture_t* render_texture = graphics_get_render_texture();
    graphics_texture_set_pixel(render_texture, x, y, color);

    return 0;
}

/**
 * Draw a line between given position and color.
 * @function line
 * @param x0 Start x-coordinate
 * @param y0 Start y-coordinate
 * @param x1 End x-coordinate
 * @param y1 End y-coordinate
 * @param color Line color
 */
static int bindings_draw_line(lua_State* L) {
    int x0 = (int)luaL_checknumber(L, 1);
    int y0 = (int)luaL_checknumber(L, 2);
    int x1 = (int)luaL_checknumber(L, 3);
    int y1 = (int)luaL_checknumber(L, 4);

    if (lua_isnumber(L, 5)) {
        int color = (int)luaL_checknumber(L, 5);
        draw_line(x0, y0, x1, y1, color);
    }
    else {
        texture_t** pattern = luaL_checktexture(L, 5);
        int offset_x = (int)luaL_optnumber(L, 6, 0);
        int offset_y = (int)luaL_optnumber(L, 7, 0);
        draw_pattern_line(x0, y0, x1, y1, *pattern, offset_x, offset_y);
    }

    lua_pop(L, -1);

    return 0;
}

/**
 * Draw rectangle.
 * @function rectangle
 * @param x Rect top left x-coordinate
 * @param y Rect top left y-coordinate
 * @param width Rect width
 * @param height Rect height
 * @param color Line color
 */
static int bindings_draw_rectangle(lua_State* L) {
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int width = (int)luaL_checknumber(L, 3);
    int height = (int)luaL_checknumber(L, 4);

    if (lua_isnumber(L, 5)) {
        int color = (int)luaL_checknumber(L, 5);
        draw_rectangle(x, y, width, height, color);
    }
    else {
        texture_t** pattern = luaL_checktexture(L, 5);
        int offset_x = (int)luaL_optnumber(L, 6, 0);
        int offset_y = (int)luaL_optnumber(L, 7, 0);
        draw_pattern_rectangle(x, y, width, height, *pattern, offset_x, offset_y);
    }

    lua_pop(L, -1);

    return 0;
}

/**
 * Draw filled rectangle.
 * @function filled_rectangle
 * @param x Rect top left x-coordinate
 * @param y Rect top left y-coordinate
 * @param width Rect width
 * @param height Rect height
 * @param color Fill color
 */
static int bindings_draw_filled_rectangle(lua_State* L) {
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int width = (int)luaL_checknumber(L, 3);
    int height = (int)luaL_checknumber(L, 4);

    if (lua_isnumber(L, 5)) {
        int color = (int)luaL_checknumber(L, 5);
        draw_filled_rectangle(x, y, width, height, color);
    }
    else {
        texture_t** pattern = luaL_checktexture(L, 5);
        int offset_x = (int)luaL_optnumber(L, 6, 0);
        int offset_y = (int)luaL_optnumber(L, 7, 0);
        draw_filled_pattern_rectangle(x, y, width, height, *pattern, offset_x, offset_y);
    }

    lua_pop(L, -1);

    return 0;
}

/**
 * Draw circle
 * @function circle
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param color Line color
 */
static int bindings_draw_circle(lua_State* L) {
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int radius = (int)luaL_checknumber(L, 3);

    if (lua_isnumber(L, 4)) {
        int color = (int)luaL_checknumber(L, 4);
        draw_circle(x, y, radius, color);
    }
    else {
        texture_t** pattern = luaL_checktexture(L, 4);
        int offset_x = (int)luaL_optnumber(L, 5, 0);
        int offset_y = (int)luaL_optnumber(L, 6, 0);
        draw_pattern_circle(x, y, radius, *pattern, offset_x, offset_y);
    }

    lua_pop(L, -1);

    return 0;
}

/**
 * Draw filled circle
 * @function filled_circle
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param color Fill color
 */
static int bindings_draw_filled_circle(lua_State* L) {
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int radius = (int)luaL_checknumber(L, 3);

    if (lua_isnumber(L, 4)) {
        int color = (int)luaL_checknumber(L, 4);
        draw_filled_circle(x, y, radius, color);
    }
    else {
        texture_t** pattern = luaL_checktexture(L, 4);
        int offset_x = (int)luaL_optnumber(L, 5, 0);
        int offset_y = (int)luaL_optnumber(L, 6, 0);
        draw_filled_pattern_circle(x, y, radius, *pattern, offset_x, offset_y);
    }

    lua_pop(L, -1);

    return 0;
}

/**
 * Clear screen to given color.
 * @function clear
 * @param color Color to clear screen
 */
static int bindings_clear_screen(lua_State* L) {
    int color = (int)luaL_checknumber(L, 1);

    lua_pop(L, -1);

    texture_t* render_texture = graphics_get_render_texture();
    graphics_texture_clear(render_texture, color);

    return 0;
}

/**
 * Draw text to screen.
 * @function text
 * @param message Text to draw
 * @param x Text top-left x-coordinate
 * @param y Text top-left y-coordinate
 */
static int bindings_draw_text(lua_State* L) {
    const char* message = (const char*)luaL_checkstring(L, 1);
    int x = (int)luaL_checknumber(L, 2);
    int y = (int)luaL_checknumber(L, 3);

    lua_pop(L, -1);

    draw_text(message, x, y);

    return 0;
}

static const struct luaL_Reg module_functions[] = {
    {"pixel", bindings_draw_pixel},
    {"line", bindings_draw_line},
    {"rectangle", bindings_draw_rectangle},
    {"filled_rectangle", bindings_draw_filled_rectangle},
    {"circle", bindings_draw_circle},
    {"filled_circle", bindings_draw_filled_circle},
    {"clear", bindings_clear_screen},
    {"text", bindings_draw_text},
    {NULL, NULL}
};

int luaopen_draw(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
