#include <SDL.h>
#include <SDL_mixer.h>
#include <emscripten.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

static SDL_Window* window_ = NULL;

static int modules_desktop_window_position_set(lua_State* L) {
    int x = luaL_checknumber(L, 1);
    int y = luaL_checknumber(L, 2);
    SDL_SetWindowPosition(window_, x, y);

    return 0;
}

static int modules_desktop_window_position_get(lua_State* L) {
    int x;
    int y;
    SDL_GetWindowPosition(window_, &x, &y);

    lua_pushinteger(L, x);
    lua_pushinteger(L, y);

    return 2;
}

static int modules_desktop_window_size_set(lua_State* L) {
    int w = luaL_checknumber(L, 1);
    int h = luaL_checknumber(L, 2);

    SDL_SetWindowSize(window_, w, h);

    return 0;
}

static int modules_desktop_window_size_get(lua_State* L) {
    int w;
    int h;
    SDL_GetWindowSize(window_, &w, &h);

    lua_pushinteger(L, w);
    lua_pushinteger(L, h);

    return 2;
}

static int modules_desktop_window_title_set(lua_State* L) {
    const char* title = luaL_checkstring(L, 1);
    SDL_SetWindowTitle(window_, title);

    return 0;
}

static int modules_desktop_window_title_get(lua_State* L) {
    const char* title = SDL_GetWindowTitle(window_);
    lua_pushstring(L, title);

    return 1;
}

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

static int modules_desktop_window_fullscreen_get(lua_State* L) {
    uint32_t flags = SDL_GetWindowFlags(window_);
    bool fullscreen = flags & SDL_WINDOW_FULLSCREEN;

    lua_pushboolean(L, fullscreen);

    return 1;
}

static const struct luaL_Reg modules_web_window_functions[] = {
    {"set_position", modules_desktop_window_position_set},
    {"get_position", modules_desktop_window_position_get},
    {"set_size", modules_desktop_window_size_set},
    {"get_size", modules_desktop_window_size_get},
    {"set_title", modules_desktop_window_title_set},
    {"get_title", modules_desktop_window_title_get},
    {"set_fullscreen", modules_desktop_window_fullscreen_set},
    {"get_fullscreen", modules_desktop_window_fullscreen_get},
    {NULL, NULL}
};

static int luaopen_platform(lua_State* L) {
    lua_newtable(L);

    lua_pushstring(L, "window");
    luaL_newlib(L, modules_web_window_functions);
    lua_settable(L, -3);

    lua_pushstring(L, "name");
    lua_pushstring(L, "web");
    lua_settable(L, -3);

    return 1;
}

void open_web_platform_module(void* arg, SDL_Window* w) {
    window_ = w;
    lua_State* L = (lua_State*)arg;

    luaL_requiref(L, "platform", luaopen_platform, 0);
    lua_pop(L, 1);
}