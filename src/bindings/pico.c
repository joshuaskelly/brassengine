#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "pico.h"
#include "../graphics.h"
#include "../renderers/draw.h"

static color_t default_color;

int color_or_default(lua_State*  L, int idx) {
    if (lua_isnumber(L, idx)) {
        return (int)lua_tonumber(L, idx);
    }

    return default_color;
}

int pico_circ(lua_State* L) {
    int x = (int)lua_tonumber(L, 1);
    int y = (int)lua_tonumber(L, 2);
    int radius = (int)lua_tonumber(L, 3);
    int color = color_or_default(L, 4);

    lua_pop(L, -1);

    draw_circle(x, y, radius, color);

    return 0;
}

int pico_circfill(lua_State* L) {
    int x = (int)lua_tonumber(L, 1);
    int y = (int)lua_tonumber(L, 2);
    int radius = (int)lua_tonumber(L, 3);
    int color = color_or_default(L, 4);

    lua_pop(L, -1);

    draw_filled_circle(x, y, radius, color);

    return 0;
}

int pico_clip(lua_State* L) {
    int x = (int)lua_tonumber(L, 1);
    int y = (int)lua_tonumber(L, 2);
    int width = (int)lua_tonumber(L, 3);
    int height = (int)lua_tonumber(L, 4);

    lua_pop(L, -1);

    graphics_set_clipping_rectangle(x, y, width, height);

    return 0;
}

int pico_cls(lua_State* L) {
    int color = color_or_default(L, 1);

    lua_pop(L, -1);

    texture_t* render_texture = graphics_get_render_texture();
    graphics_texture_clear(render_texture, color);

    return 0;
}

int pico_color(lua_State* L) {
    default_color = (int)lua_tonumber(L, 1);

    lua_pop(L, -1);

    return 0;
}

int pico_line(lua_State* L) {
    int x0 = (int)lua_tonumber(L, 1);
    int y0 = (int)lua_tonumber(L, 2);
    int x1 = (int)lua_tonumber(L, 3);
    int y1 = (int)lua_tonumber(L, 4);
    int color = color_or_default(L, 5);

    lua_pop(L, -1);

    draw_line(x0, y0, x1, y1, color);

    return 0;
}

int pico_pget(lua_State* L) {
    int x = (int)lua_tonumber(L, 1);
    int y = (int)lua_tonumber(L, 2);

    lua_pop(L, -1);

    texture_t* render_texture = graphics_get_render_texture();
    color_t pixel = graphics_texture_get_pixel(render_texture, x, y);

    lua_pushinteger(L, pixel);

    return 1;
}

int pico_pset(lua_State* L) {
    int x = (int)lua_tonumber(L, 1);
    int y = (int)lua_tonumber(L, 2);
    int color = color_or_default(L, 3);

    lua_pop(L, -1);

    graphics_set_pixel(x, y, color);

    return 0;
}

int pico_rect(lua_State* L) {
    int x = (int)lua_tonumber(L, 1);
    int y = (int)lua_tonumber(L, 2);
    int width = (int)lua_tonumber(L, 3);
    int height = (int)lua_tonumber(L, 4);
    int color = color_or_default(L, 5);

    lua_pop(L, -1);

    draw_rectangle(x, y, width, height, color);

    return 0;
}

int pico_rectfill(lua_State* L) {
    int x = (int)lua_tonumber(L, 1);
    int y = (int)lua_tonumber(L, 2);
    int width = (int)lua_tonumber(L, 3);
    int height = (int)lua_tonumber(L, 4);
    int color = color_or_default(L, 5);

    lua_pop(L, -1);

    draw_filled_rectangle(x, y, width, height, color);

    return 0;
}

static const struct luaL_Reg pico_module_functions[] = {
    {"circ", pico_circ},
    {"circfill", pico_circfill},
    {"clip", pico_clip},
    {"cls", pico_cls},
    {"color", pico_color},
    {"line", pico_line},
    {"pget", pico_pget},
    {"pset", pico_pset},
    {"rect", pico_rect},
    {"rectfill", pico_rectfill},
    {NULL, NULL},
};

int open_pico_module(lua_State* L) {
    luaL_newlib(L, pico_module_functions);
    return 1;
}
