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

static texture_t* draw_render_texture_get(void);
static void draw_render_texture_set(texture_t* texture);

/**
 * Draw a pixel at given position and color.
 * @function pixel
 * @tparam integer x Pixel x-coordinate
 * @tparam integer y Pixel y-coordinate
 * @tparam integer color Pixel color
 */
static int modules_draw_pixel(lua_State* L) {
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int color = (int)luaL_checknumber(L, 3);

    lua_settop(L, 0);

    texture_t* render_texture = draw_render_texture_get();
    graphics_draw_pixel(render_texture, x, y, color);

    return 0;
}

/**
 * Draw a line between given position and color.
 * @function line
 * @tparam integer x0 Start x-coordinate
 * @tparam integer y0 Start y-coordinate
 * @tparam integer x1 End x-coordinate
 * @tparam integer y1 End y-coordinate
 * @tparam integer color Line color
 */
static int modules_draw_line(lua_State* L) {
    int x0 = (int)luaL_checknumber(L, 1);
    int y0 = (int)luaL_checknumber(L, 2);
    int x1 = (int)luaL_checknumber(L, 3);
    int y1 = (int)luaL_checknumber(L, 4);

    texture_t* render_texture = draw_render_texture_get();

    if (lua_isnumber(L, 5)) {
        int color = (int)luaL_checknumber(L, 5);
        graphics_draw_line(render_texture, x0, y0, x1, y1, color);
    }
    else {
        texture_t* pattern = luaL_checktexture(L, 5);
        int offset_x = (int)luaL_optnumber(L, 6, 0);
        int offset_y = (int)luaL_optnumber(L, 7, 0);
        graphics_draw_pattern_line(render_texture, x0, y0, x1, y1, pattern, offset_x, offset_y);
    }

    lua_settop(L, 0);

    return 0;
}

/**
 * Draw line using affine texture mapping.
 * @function textured_line
 * @tparam integer x0 Start x-coordinate
 * @tparam integer y0 Start y-coordinate
 * @tparam number u0 Start 0 u-coordinate
 * @tparam number v0 Start 0 v-coordinate
 * @tparam integer x1 End x-coordinate
 * @tparam integer y1 End y-coordinate
 * @tparam number u1 End u-coordinate
 * @tparam number v1 End v-coordinate
 * @tparam texture.texture texture Texture to map
 */
static int modules_draw_textured_line(lua_State* L) {
    int x0 = (int)luaL_checknumber(L, 1);
    int y0 = (int)luaL_checknumber(L, 2);
    float u0 = luaL_checknumber(L, 3);
    float v0 = luaL_checknumber(L, 4);
    int x1 = (int)luaL_checknumber(L, 5);
    int y1 = (int)luaL_checknumber(L, 6);
    float u1 = luaL_checknumber(L, 7);
    float v1 = luaL_checknumber(L, 8);
    texture_t* texture = luaL_checktexture(L, 9);

    lua_settop(L, 0);

    texture_t* render_texture = draw_render_texture_get();
    graphics_draw_textured_line(render_texture, x0, y0, u0, v0, x1, y1, u1, v1, texture);

    return 0;
}

/**
 * Draw a bezier curve with the given anchor and control points, and color.
 * @function bezier
 * @tparam integer x0 Start anchor point x-coordinate
 * @tparam integer y0 Start anchor point y-coordinate
 * @tparam integer x1 Start control point x-coordinate
 * @tparam integer y1 Start contorl point y-coordinate
 * @tparam integer x2 End control point x-coordinate
 * @tparam integer y2 End control point y-coordinate
 * @tparam integer x3 End anchor point x-coordinate
 * @tparam integer y3 End anchor point y-coordinate
 * @tparam integer color Line color
 */
static int modules_draw_bezier(lua_State* L) {
    int x0 = (int)luaL_checknumber(L, 1);
    int y0 = (int)luaL_checknumber(L, 2);
    int x1 = (int)luaL_checknumber(L, 3);
    int y1 = (int)luaL_checknumber(L, 4);
    int x2 = (int)luaL_checknumber(L, 5);
    int y2 = (int)luaL_checknumber(L, 6);
    int x3 = (int)luaL_checknumber(L, 7);
    int y3 = (int)luaL_checknumber(L, 8);

    texture_t* render_texture = draw_render_texture_get();

    if (lua_isnumber(L, 9)) {
        int color = (int)luaL_checknumber(L, 9);
        graphics_draw_bezier(render_texture, x0, y0, x1, y1, x2, y2, x3, y3, color);
    }
    else {
        texture_t* pattern = luaL_checktexture(L, 9);
        int offset_x = (int)luaL_optnumber(L, 10, 0);
        int offset_y = (int)luaL_optnumber(L, 11, 0);
        graphics_draw_pattern_bezier(render_texture, x0, y0, x1, y1, x2, y2, x3, y3, pattern, offset_x, offset_y);
    }

    lua_settop(L, 0);

    return 0;
}

/**
 * Draw rectangle.
 * @function rectangle
 * @tparam integer x Rect top left x-coordinate
 * @tparam integer y Rect top left y-coordinate
 * @tparam integer width Rect width
 * @tparam integer height Rect height
 * @tparam integer color Line color
 */
static int modules_draw_rectangle(lua_State* L) {
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int width = (int)luaL_checknumber(L, 3);
    int height = (int)luaL_checknumber(L, 4);

    texture_t* render_texture = draw_render_texture_get();

    if (lua_isnumber(L, 5)) {
        int color = (int)luaL_checknumber(L, 5);
        graphics_draw_rectangle(render_texture, x, y, width, height, color);
    }
    else {
        texture_t* pattern = luaL_checktexture(L, 5);
        int offset_x = (int)luaL_optnumber(L, 6, 0);
        int offset_y = (int)luaL_optnumber(L, 7, 0);
        graphics_draw_pattern_rectangle(render_texture, x, y, width, height, pattern, offset_x, offset_y);
    }

    lua_settop(L, 0);

    return 0;
}

/**
 * Draw filled rectangle.
 * @function filled_rectangle
 * @tparam integer x Rect top left x-coordinate
 * @tparam integer y Rect top left y-coordinate
 * @tparam integer width Rect width
 * @tparam integer height Rect height
 * @tparam integer color Fill color
 */
static int modules_draw_filled_rectangle(lua_State* L) {
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int width = (int)luaL_checknumber(L, 3);
    int height = (int)luaL_checknumber(L, 4);

    texture_t* render_texture = draw_render_texture_get();

    if (lua_isnumber(L, 5)) {
        int color = (int)luaL_checknumber(L, 5);
        graphics_draw_filled_rectangle(render_texture, x, y, width, height, color);
    }
    else {
        texture_t* pattern = luaL_checktexture(L, 5);
        int offset_x = (int)luaL_optnumber(L, 6, 0);
        int offset_y = (int)luaL_optnumber(L, 7, 0);
        graphics_draw_filled_pattern_rectangle(render_texture, x, y, width, height, pattern, offset_x, offset_y);
    }

    lua_settop(L, 0);

    return 0;
}

/**
 * Draw circle
 * @function circle
 * @tparam integer x Circle center x-coordinate
 * @tparam integer y Circle center y-coordinate
 * @tparam integer radius Circle radius
 * @tparam integer color Line color
 */
static int modules_draw_circle(lua_State* L) {
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int radius = (int)luaL_checknumber(L, 3);

    texture_t* render_texture = draw_render_texture_get();

    if (lua_isnumber(L, 4)) {
        int color = (int)luaL_checknumber(L, 4);
        graphics_draw_circle(render_texture, x, y, radius, color);
    }
    else {
        texture_t* pattern = luaL_checktexture(L, 4);
        int offset_x = (int)luaL_optnumber(L, 5, 0);
        int offset_y = (int)luaL_optnumber(L, 6, 0);
        graphics_draw_pattern_circle(render_texture, x, y, radius, pattern, offset_x, offset_y);
    }

    lua_settop(L, 0);

    return 0;
}

/**
 * Draw filled circle
 * @function filled_circle
 * @tparam integer x Circle center x-coordinate
 * @tparam integer y Circle center y-coordinate
 * @tparam integer radius Circle radius
 * @tparam integer color Fill color
 */
static int modules_draw_filled_circle(lua_State* L) {
    int x = (int)luaL_checknumber(L, 1);
    int y = (int)luaL_checknumber(L, 2);
    int radius = (int)luaL_checknumber(L, 3);

    texture_t* render_texture = draw_render_texture_get();

    if (lua_isnumber(L, 4)) {
        int color = (int)luaL_checknumber(L, 4);
        graphics_draw_filled_circle(render_texture, x, y, radius, color);
    }
    else {
        texture_t* pattern = luaL_checktexture(L, 4);
        int offset_x = (int)luaL_optnumber(L, 5, 0);
        int offset_y = (int)luaL_optnumber(L, 6, 0);
        graphics_draw_filled_pattern_circle(render_texture, x, y, radius, pattern, offset_x, offset_y);
    }

    lua_settop(L, 0);

    return 0;
}

/**
 * Clear screen to given color.
 * @function clear
 * @tparam integer color Color to clear screen
 */
static int modules_clear_screen(lua_State* L) {
    int color = (int)luaL_checknumber(L, 1);

    lua_settop(L, 0);

    texture_t* render_texture = draw_render_texture_get();
    graphics_texture_clear(render_texture, color);

    return 0;
}

/**
 * Draw text to screen.
 * @function text
 * @tparam string message Text to draw
 * @tparam integer x Text top-left x-coordinate
 * @tparam integer y Text top-left y-coordinate
 * @tparam ?integer foreground Foreground color
 * @tparam ?integer background Background color
 */
static int modules_draw_text(lua_State* L) {
    color_t* palette = graphics_draw_palette_get();

    const char* message = (const char*)luaL_checkstring(L, 1);
    int x = (int)luaL_checknumber(L, 2);
    int y = (int)luaL_checknumber(L, 3);
    int fg = (int)luaL_optnumber(L, 4, palette[1]);
    int bg = (int)luaL_optnumber(L, 5, palette[0]);

    lua_settop(L, 0);

    int bg_old = palette[0];
    int fg_old = palette[1];

    palette[0] = bg;
    palette[1] = fg;

    texture_t* render_texture = draw_render_texture_get();
    graphics_draw_text(render_texture, message, x, y);

    palette[0] = bg_old;
    palette[1] = fg_old;

    return 0;
}

/**
 * Draw triangle.
 * @function triangle
 * @tparam integer x0 Vertex 0 x-coordinate
 * @tparam integer y0 Vertex 0 y-coordinate
 * @tparam integer x1 Vertex 1 x-coordinate
 * @tparam integer y1 Vertex 1 y-coordinate
 * @tparam integer x2 Vertex 2 x-coordinate
 * @tparam integer y2 Vertex 2 y-coordinate
 * @tparam integer color Line color
 */
static int modules_draw_triangle(lua_State* L) {
    int x0 = (int)luaL_checknumber(L, 1);
    int y0 = (int)luaL_checknumber(L, 2);
    int x1 = (int)luaL_checknumber(L, 3);
    int y1 = (int)luaL_checknumber(L, 4);
    int x2 = (int)luaL_checknumber(L, 5);
    int y2 = (int)luaL_checknumber(L, 6);

    texture_t* render_texture = draw_render_texture_get();

    if (lua_isnumber(L, 7)) {
        int color = (int)luaL_checknumber(L, 7);
        graphics_draw_triangle(render_texture, x0, y0, x1, y1, x2, y2, color);
    }
    else {
        texture_t* pattern = luaL_checktexture(L, 7);
        int offset_x = (int)luaL_optnumber(L, 8, 0);
        int offset_y = (int)luaL_optnumber(L, 9, 0);
        graphics_draw_pattern_triangle(render_texture, x0, y0, x1, y1, x2, y2, pattern, offset_x, offset_y);
    }

    lua_settop(L, 0);

    return 0;
}

/**
 * Draw filled triangle.
 * @function filled_triangle
 * @tparam integer x0 Vertex 0 x-coordinate
 * @tparam integer y0 Vertex 0 y-coordinate
 * @tparam integer x1 Vertex 1 x-coordinate
 * @tparam integer y1 Vertex 1 y-coordinate
 * @tparam integer x2 Vertex 2 x-coordinate
 * @tparam integer y2 Vertex 2 y-coordinate
 * @tparam integer color Fill color
 */
static int modules_draw_filled_triangle(lua_State* L) {
    int x0 = (int)luaL_checknumber(L, 1);
    int y0 = (int)luaL_checknumber(L, 2);
    int x1 = (int)luaL_checknumber(L, 3);
    int y1 = (int)luaL_checknumber(L, 4);
    int x2 = (int)luaL_checknumber(L, 5);
    int y2 = (int)luaL_checknumber(L, 6);

    texture_t* render_texture = draw_render_texture_get();

    if (lua_isnumber(L, 7)) {
        int color = (int)luaL_checknumber(L, 7);
        graphics_draw_filled_triangle(render_texture, x0, y0, x1, y1, x2, y2, color);
    }
    else {
        texture_t* pattern = luaL_checktexture(L, 7);
        int offset_x = (int)luaL_optnumber(L, 8, 0);
        int offset_y = (int)luaL_optnumber(L, 9, 0);
        graphics_draw_filled_pattern_triangle(render_texture, x0, y0, x1, y1, x2, y2, pattern, offset_x, offset_y);
    }

    lua_settop(L, 0);

    return 0;
}

/**
 * Draw triangle using affine texture mapping.
 * @function textured_triangle
 * @tparam integer x0 Vertex 0 x-coordinate
 * @tparam integer y0 Vertex 0 y-coordinate
 * @tparam number u0 UV 0 u-coordinate
 * @tparam number v0 UV 0 v-coordinate
 * @tparam integer x1 Vertex 1 x-coordinate
 * @tparam integer y1 Vertex 1 y-coordinate
 * @tparam number u1 UV 1 u-coordinate
 * @tparam number v1 UV 1 v-coordinate
 * @tparam integer x2 Vertex 2 x-coordinate
 * @tparam integer y2 Vertex 2 y-coordinate
 * @tparam number u2 UV 2 u-coordinate
 * @tparam number v2 UV 2 v-coordinate
 * @tparam texture.texture texture Texture to map
 */
static int modules_draw_textured_triangle(lua_State* L) {
    int x0 = (int)luaL_checknumber(L, 1);
    int y0 = (int)luaL_checknumber(L, 2);
    float u0 = luaL_checknumber(L, 3);
    float v0 = luaL_checknumber(L, 4);
    int x1 = (int)luaL_checknumber(L, 5);
    int y1 = (int)luaL_checknumber(L, 6);
    float u1 = luaL_checknumber(L, 7);
    float v1 = luaL_checknumber(L, 8);
    int x2 = (int)luaL_checknumber(L, 9);
    int y2 = (int)luaL_checknumber(L, 10);
    float u2 = luaL_checknumber(L, 11);
    float v2 = luaL_checknumber(L, 12);
    texture_t* texture = luaL_checktexture(L, 13);

    lua_settop(L, 0);

    texture_t* render_texture = draw_render_texture_get();
    graphics_draw_textured_triangle(render_texture, x0, y0, u0, v0, x1, y1, u1, v1, x2, y2, u2, v2, texture);

    return 0;
}

static texture_t* render_texture = NULL;
static texture_t* draw_render_texture_get(void) {
    if (!render_texture) {
        return graphics_render_texture_get();
    }

    return render_texture;
}

static void draw_render_texture_set(texture_t* texture) {
    render_texture = texture;
}

/**
 * Get current render texture for drawing
 * @function get_render_texture
 * @return texture.texture Current drawing render texture
 */
static int modules_draw_render_texture_get(lua_State* L) {
    lua_pushtexture(L, draw_render_texture_get());
    return 1;
}

/**
 * Set render texture for drawing
 * @tparam ?texture.texture Texture to set as render texture. Calling with no
 * param will reset drawing back to graphics render texture.
 */
static int modules_draw_render_texture_set(lua_State* L) {
    if (lua_gettop(L) == 0 || lua_isnil(L, 1)) {
        draw_render_texture_set(NULL);
        return 0;
    }

    texture_t* texture = luaL_checktexture(L, 1);
    draw_render_texture_set(texture);

    return 0;
}


static const struct luaL_Reg modules_draw_functions[] = {
    {"pixel", modules_draw_pixel},
    {"line", modules_draw_line},
    {"textured_line", modules_draw_textured_line},
    {"bezier", modules_draw_bezier},
    {"rectangle", modules_draw_rectangle},
    {"filled_rectangle", modules_draw_filled_rectangle},
    {"circle", modules_draw_circle},
    {"filled_circle", modules_draw_filled_circle},
    {"clear", modules_clear_screen},
    {"text", modules_draw_text},
    {"triangle", modules_draw_triangle},
    {"filled_triangle", modules_draw_filled_triangle},
    {"textured_triangle", modules_draw_textured_triangle},
    {"get_render_texture", modules_draw_render_texture_get},
    {"set_render_texture", modules_draw_render_texture_set},
    {NULL, NULL}
};

int luaopen_draw(lua_State* L) {
    luaL_newlib(L, modules_draw_functions);
    return 1;
}
