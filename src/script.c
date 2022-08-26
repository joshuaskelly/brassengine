#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "event.h"
#include "log.h"
#include "script.h"

static lua_State* L = NULL;

static bool buttons[2];

int api_print(lua_State* L);
int api_button(lua_State* L);

void script_init(void) {
    log_info("script init\n");

    // Create Lua VM
    L = luaL_newstate();

    if (!L) {
        log_fatal("Error creating Lua VM.\n");
    }

    luaL_openlibs(L);

    // Set globals
    lua_pushcfunction(L, api_print);
    lua_setglobal(L, "print");

    lua_pushcfunction(L, api_button);
    lua_setglobal(L, "button");

    // Execute lua script
    luaL_dofile(L, "./assets/script.lua");

    buttons[0] = false;
    buttons[1] = false;
}

void script_destroy(void) {
    lua_close(L);
}

bool script_handle_event(event_t* event) {
    switch (event->type) {
        case EVENT_KEYDOWN:
            if (event->key.symbol == KEYSYMBOL_a) {
                buttons[0] = true;
                return true;
            }
            else if (event->key.symbol == KEYSYMBOL_s) {
                buttons[1] = true;
                return true;
            }

            break;

        case EVENT_KEYUP:
            if (event->key.symbol == KEYSYMBOL_a) {
                buttons[0] = false;
                return true;
            }
            else if (event->key.symbol == KEYSYMBOL_s) {
                buttons[1] = false;
                return true;
            }

            break;

        case EVENT_QUIT:
        case EVENT_UNSET:
        default:
            break;
    }

    return false;
}

void script_update(void) {
    // Invoke global _update function
    lua_getglobal(L, "_update");
    if (lua_isfunction(L, -1)) {
        lua_pcall(L, 0, 0, 0);
    }
}

void script_setup(void) {
    // Invoke global _init function
    lua_getglobal(L, "_init");
    if (lua_isfunction(L, -1)) {
        lua_pcall(L, 0, 0, 0);
    }
}

void script_draw(void) {
    // Invoke global _draw function
    lua_getglobal(L, "_draw");
    if (lua_isfunction(L, -1)) {
        lua_pcall(L, 0, 0, 0);
    }
}

int api_print(lua_State* L) {
    const char* message = lua_tostring(L, -1);
    printf("%s\n", message);
    lua_pop(L, -1);

    return 0;
}

int api_button(lua_State* L) {
    if (lua_isnumber(L, -1)) {
        int buttonId = (int)lua_tonumber(L, -1);
        lua_pop(L, -1);
        lua_pushboolean(L, buttons[buttonId]);
    }
    else {
        lua_pushboolean(L, false);
    }

    return 1;
}
