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
int bindings_draw_pixel(lua_State* L) {
    int x = (int)lua_tonumber(L, -3);
    int y = (int)lua_tonumber(L, -2);
    int color = (int)lua_tonumber(L, -1);

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
int bindings_draw_line(lua_State* L) {
    int x0 = (int)lua_tonumber(L, -5);
    int y0 = (int)lua_tonumber(L, -4);
    int x1 = (int)lua_tonumber(L, -3);
    int y1 = (int)lua_tonumber(L, -2);
    int color = (int)lua_tonumber(L, -1);

    lua_pop(L, -1);

    draw_line(x0, y0, x1, y1, color);

    return 0;
}

int bindings_draw_rectangle(lua_State* L) {
    int x = (int)lua_tonumber(L, -5);
    int y = (int)lua_tonumber(L, -4);
    int width = (int)lua_tonumber(L, -3);
    int height = (int)lua_tonumber(L, -2);
    int color = (int)lua_tonumber(L, -1);

    lua_pop(L, -1);

    draw_rectangle(x, y, width, height, color);

    return 0;
}

int bindings_draw_filled_rectangle(lua_State* L) {
    int x = (int)lua_tonumber(L, -5);
    int y = (int)lua_tonumber(L, -4);
    int width = (int)lua_tonumber(L, -3);
    int height = (int)lua_tonumber(L, -2);
    int color = (int)lua_tonumber(L, -1);

    lua_pop(L, -1);

    draw_filled_rectangle(x, y, width, height, color);

    return 0;
}

int bindings_draw_circle(lua_State* L) {
    int x = (int)lua_tonumber(L, -4);
    int y = (int)lua_tonumber(L, -3);
    int radius = (int)lua_tonumber(L, -2);
    int color = (int)lua_tonumber(L, -1);

    lua_pop(L, -1);

    draw_circle(x, y, radius, color);

    return 0;
}

int bindings_draw_filled_circle(lua_State* L) {
    int x = (int)lua_tonumber(L, -4);
    int y = (int)lua_tonumber(L, -3);
    int radius = (int)lua_tonumber(L, -2);
    int color = (int)lua_tonumber(L, -1);

    lua_pop(L, -1);

    draw_filled_circle(x, y, radius, color);

    return 0;
}

/**
 * Clear screen to given color.
 *
 * @param color Color to clear screen
 */
int bindings_clear_screen(lua_State* L) {
    int color = (int)lua_tonumber(L, -1);

    lua_pop(L, -1);

    texture_t* render_texture = graphics_get_render_texture();
    graphics_texture_clear(render_texture, color);

    return 0;
}

int bindings_test_blit(lua_State* L) {
    int sx = (int)lua_tonumber(L, 1);
    int sy = (int)lua_tonumber(L, 2);
    int sw = (int)lua_tonumber(L, 3);
    int sh = (int)lua_tonumber(L, 4);
    int dx = (int)lua_tonumber(L, 5);
    int dy = (int)lua_tonumber(L, 6);
    int dw = (int)lua_tonumber(L, 7);
    int dh = (int)lua_tonumber(L, 8);

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
    {"test_blit", bindings_test_blit},
    {NULL, NULL}
};

int open_draw_module(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}