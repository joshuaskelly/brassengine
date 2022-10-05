#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "draw.h"
#include "../assets.h"
#include "../graphics.h"
#include "../renderers/draw.h"

/**
 * Draw a pixel at given position and color.
 *
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
 *
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
    int color = (int)luaL_checknumber(L, 5);

    lua_pop(L, -1);

    draw_line(x0, y0, x1, y1, color);

    return 0;
}

/**
 * Draw rectangle.
 *
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
    int color = (int)luaL_checknumber(L, 5);

    lua_pop(L, -1);

    draw_rectangle(x, y, width, height, color);

    return 0;
}

/**
 * Draw filled rectangle.
 *
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
    int color = (int)luaL_checknumber(L, 5);

    lua_pop(L, -1);

    draw_filled_rectangle(x, y, width, height, color);

    return 0;
}

/**
 * Draw circle
 *
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param color Line color
 */
static int bindings_draw_circle(lua_State* L) {
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int radius = (int)luaL_checknumber(L, 3);
    int color = (int)luaL_checknumber(L, 4);

    lua_pop(L, -1);

    draw_circle(x, y, radius, color);

    return 0;
}

/**
 * Draw filled circle
 *
 * @param x Circle center x-coordinate
 * @param y Circle center y-coordinate
 * @param radius Circle radius
 * @param color Fill color
 */
static int bindings_draw_filled_circle(lua_State* L) {
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int radius = (int)luaL_checknumber(L, 3);
    int color = (int)luaL_checknumber(L, 4);

    lua_pop(L, -1);

    draw_filled_circle(x, y, radius, color);

    return 0;
}

/**
 * Clear screen to given color.
 *
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
 *
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

static int bindings_test_blit(lua_State* L) {
    int sx = (int)luaL_checknumber(L, 1);
    int sy = (int)luaL_checknumber(L, 2);
    int sw = (int)luaL_checknumber(L, 3);
    int sh = (int)luaL_checknumber(L, 4);
    int dx = (int)luaL_checknumber(L, 5);
    int dy = (int)luaL_checknumber(L, 6);
    int dw = (int)luaL_checknumber(L, 7);
    int dh = (int)luaL_checknumber(L, 8);

    lua_pop(L, -1);

    rect_t source_rect = {sx, sy, sw, sh};
    rect_t dest_rect = {dx, dy, dw, dh};
    texture_t* render_texture = graphics_get_render_texture();

    // TODO: Fix this hard coding of main texture
    texture_t* source_texture = assets_get_texture("textures.gif");

    if (source_texture) {
        graphics_texture_blit(source_texture, render_texture, &source_rect, &dest_rect);
    }

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
    {"test_blit", bindings_test_blit},
    {NULL, NULL}
};

int open_draw_module(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}