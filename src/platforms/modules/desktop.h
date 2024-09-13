/**
 * Module for getting keyboard input.
 * @module keyboard
 */
#include <ctype.h>
#include <stdbool.h>

#include <SDL.h>
#include <SDL_mixer.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

static SDL_Window* window_ = NULL;
static SDL_Renderer* renderer_ = NULL;

/**
 * Check if given key is down.
 * @function key
 * @tparam integer|string code_or_symbol Key code or symbol to check.
 * @treturn boolean True if key is down.
 */
static int test(lua_State* L) {
    SDL_SetWindowPosition(window_, 0, 0);

    return 0;
}

static const struct luaL_Reg modules_test_functions[] = {
    {"test", test},
    {NULL, NULL}
};

static int luaopen_platform(lua_State* L) {
    luaL_newlib(L, modules_test_functions);
    return 1;
}

void open_desktop_platform_module(void* arg, SDL_Window* w, SDL_Renderer* r) {
    window_ = w;
    renderer_ = r;
    lua_State* L = (lua_State*)arg;

    luaL_requiref(L, "platform", luaopen_platform, 0);
    lua_pop(L, 1);
}
