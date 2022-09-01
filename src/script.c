#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "event.h"
#include "graphics.h"
#include "log.h"
#include "script.h"

static lua_State* L = NULL;

static bool buttons[2];
static int mouse_position[2];

int api_print(lua_State* L);
int api_button(lua_State* L);
int api_mouse_position(lua_State* L);
int api_draw_pixel(lua_State* L);
int api_clear_screen(lua_State* L);

/**
 * @brief Add a global function to Lua VM
 *
 * @param L Lua VM
 * @param function C function pointer to push
 * @param function_name Name to give pushed global function
 */
void add_global_function(lua_State* L, lua_CFunction function, const char* function_name) {
    lua_pushcfunction(L, function);
    lua_setglobal(L, function_name);
}

void script_init(void) {
    log_info("script init");

    // Create Lua VM
    L = luaL_newstate();

    if (!L) {
        log_fatal("Error creating Lua VM");
    }

    luaL_openlibs(L);

    // Set globals
    add_global_function(L, api_print, "print");
    add_global_function(L, api_button, "button");
    add_global_function(L, api_mouse_position, "mouse_position");
    add_global_function(L, api_draw_pixel, "draw_pixel");
    add_global_function(L, api_clear_screen, "clear");

    // Execute Lua script
    int result = luaL_dofile(L, "./assets/script.lua");

    if (result != LUA_OK) {
        const char* error_message = lua_tostring(L, -1);
        log_error(error_message);

        // Remove error message from stack
        lua_pop(L, -1);
    }

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

        case EVENT_MOUSEMOTION:
            mouse_position[0] = event->motion.x;
            mouse_position[1] = event->motion.y;
            break;

        case EVENT_QUIT:
        case EVENT_UNSET:
        default:
            break;
    }

    return false;
}

/**
 * @brief Call a globally defined Lua function. The function
 * must take no arguments and return no values.
 *
 * @param L Lua VM
 * @param function_name Name of the function to call
 */
void call_global_lua_function(lua_State* L, const char* function_name) {
    // Attempt to find the global object
    lua_getglobal(L, function_name);

    if (lua_isfunction(L, -1)) {
        lua_pcall(L, 0, 0, 0);
    }
    else {
        // Clean up stack if we don't get the function
        lua_pop(L, -1);
    }
}

void script_update(void) {
    call_global_lua_function(L, "_update");
}

void script_setup(void) {
    call_global_lua_function(L, "_init");
}

void script_draw(void) {
    call_global_lua_function(L, "_draw");
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

int api_mouse_position(lua_State* L) {
    lua_pushnumber(L, mouse_position[0]);
    lua_pushnumber(L, mouse_position[1]);
    return 2;
}

int api_draw_pixel(lua_State* L) {
    int x = (int)lua_tonumber(L, -3);
    int y = (int)lua_tonumber(L, -2);
    int color = (int)lua_tonumber(L, -1);

    lua_pop(L, -1);
    lua_pop(L, -1);
    lua_pop(L, -1);

    texture_t* render_texture = graphics_get_render_texture();
    texture_set_pixel(render_texture, x, y, color);

    return 0;
}

int api_clear_screen(lua_State* L) {
    int color = (int)lua_tonumber(L, -1);
    lua_pop(L, -1);

    texture_t* render_texture = graphics_get_render_texture();
    texture_clear(render_texture, color);

    return 0;
}