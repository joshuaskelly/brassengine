/**
 * Module for desktop platform specific APIs.
 *
 * @usage
 * platform = require("platform")
 *
 * platform.window.set_title("Example")
 *
 * @module platform-desktop
 */

#include <ctype.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_mixer.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "../texture.h"

#include "../../configuration.h"
#include "../../graphics.h"

#include "../../platforms/extensions/sdl-extensions.h"

static SDL_Window* window_ = NULL;

/**
 * Platform name. Should be 'desktop'
 * @tfield string name
 */

/**
 * Submodule window
 * @section window
 */

/**
 * Set window position
 * @function window.set_position
 * @tparam integer x Window left x position
 * @tparam integer y Window top y position
 */
static int modules_desktop_window_position_set(lua_State* L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    SDL_SetWindowPosition(window_, x, y);

    return 0;
}

/**
 * Get window position
 * @function window.get_position
 * @treturn integer Window left position
 * @treturn integer Window top position
 */
static int modules_desktop_window_position_get(lua_State* L) {
    int x;
    int y;
    SDL_GetWindowPosition(window_, &x, &y);

    lua_pushinteger(L, x);
    lua_pushinteger(L, y);

    return 2;
}

/**
 * Set window size
 * @function window.set_size
 * @tparam integer width Window width
 * @tparam integer height Window height
 */
static int modules_desktop_window_size_set(lua_State* L) {
    int w = luaL_checknumber(L, 1);
    int h = luaL_checknumber(L, 2);

    SDL_SetWindowSizeInPixels(window_, w, h);

    return 0;
}

/**
 * Get window size
 * @function window.get_size
 * @treturn integer width Window width
 * @treturn integer height Window height
 */
static int modules_desktop_window_size_get(lua_State* L) {
    int w;
    int h;
    SDL_GetWindowSizeInPixels(window_, &w, &h);

    lua_pushinteger(L, w);
    lua_pushinteger(L, h);

    return 2;
}

/**
 * Set window title
 * @function window.set_title
 * @tparam string title Window title
 */
static int modules_desktop_window_title_set(lua_State* L) {
    const char* title = luaL_checkstring(L, 1);
    SDL_SetWindowTitle(window_, title);

    return 0;
}

/**
 * Get window title
 * @function window.get_title
 * @treturn string Window title
 */
static int modules_desktop_window_title_get(lua_State* L) {
    const char* title = SDL_GetWindowTitle(window_);
    lua_pushstring(L, title);

    return 1;
}

/**
 * Set window fullscreen state
 * @function window.set_fullscreen
 * @tparam boolean state True to set fullscreen, false for windowed.
 */
static int modules_desktop_window_fullscreen_set(lua_State* L) {
    bool state = lua_toboolean(L, 1);

    if (state) {
        SDL_SetWindowFullscreen(window_, SDL_WINDOW_FULLSCREEN);
    }
    else {
        SDL_SetWindowFullscreen(window_, 0);
    }

    return 0;
}

/**
 * Get window fullscreen state
 * @function window.get_fullscreen
 * @treturn boolean True if fullscreen, false for windowed.
 */
static int modules_desktop_window_fullscreen_get(lua_State* L) {
    uint32_t flags = SDL_GetWindowFlags(window_);
    bool fullscreen = flags & SDL_WINDOW_FULLSCREEN;

    lua_pushboolean(L, fullscreen);

    return 1;
}

/**
 * Set window pixel aspect ratio
 * @function window.set_aspect
 * @tparam number aspect Pixel aspect ratio width / height
 */
static int modules_desktop_window_aspect_set(lua_State* L) {
    float aspect = luaL_checknumber(L, 1);
    config->display.aspect = aspect;

    return 0;
}

/**
 * Get window pixel aspect ratio
 * @function window.get_aspect
 * @treturn number Pixel aspect ratio width / height
 */
static int modules_desktop_window_aspect_get(lua_State* L) {
    lua_pushnumber(L, config->display.aspect);
    return 1;
}

/**
 * Set window icon
 * @function window.set_icon
 * @tparam texture.texture icon Window icon
 */
static int modules_desktop_window_icon_set(lua_State* L) {
    texture_t* texture = luaL_checktexture(L, 1);

    SDL_Surface* icon = SDL_CreateRGBSurfaceFrom(
        texture->pixels,
        texture->width,
        texture->height,
        8,
        texture->width,
        0, 0, 0, 0
    );

    uint32_t* palette = graphics_palette_get();
    SDL_Color colors[256];
    for (int i = 0; i < 256; i++) {
        uint32_t swatch = palette[i];
        colors[i].r = swatch & 255;
        colors[i].g = (swatch >> 8) & 255;
        colors[i].b = (swatch >> 16) & 255;
        colors[i].a = 255;
    }

    SDL_SetPaletteColors(icon->format->palette, colors, 0, 256);

    SDL_SetWindowIcon(window_, icon);

    SDL_FreeSurface(icon);

    return 0;
}

static const struct luaL_Reg modules_desktop_window_functions[] = {
    {"set_position", modules_desktop_window_position_set},
    {"get_position", modules_desktop_window_position_get},
    {"set_size", modules_desktop_window_size_set},
    {"get_size", modules_desktop_window_size_get},
    {"set_title", modules_desktop_window_title_set},
    {"get_title", modules_desktop_window_title_get},
    {"set_fullscreen", modules_desktop_window_fullscreen_set},
    {"get_fullscreen", modules_desktop_window_fullscreen_get},
    {"set_aspect", modules_desktop_window_aspect_set},
    {"get_aspect", modules_desktop_window_aspect_get},
    {"set_icon", modules_desktop_window_icon_set},
    {NULL, NULL}
};

static int luaopen_platform(lua_State* L) {
    lua_newtable(L);

    lua_pushstring(L, "window");
    luaL_newlib(L, modules_desktop_window_functions);
    lua_settable(L, -3);

    lua_pushstring(L, "name");
    lua_pushstring(L, "desktop");
    lua_settable(L, -3);

    return 1;
}

void open_desktop_platform_module(void* arg, SDL_Window* w) {
    window_ = w;
    lua_State* L = (lua_State*)arg;

    luaL_requiref(L, "platform", luaopen_platform, 0);
    lua_pop(L, 1);
}
