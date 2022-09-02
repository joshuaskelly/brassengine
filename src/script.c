#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "event.h"
#include "graphics.h"
#include "log.h"
#include "script.h"

static lua_State* L = NULL;

static bool buttons[6];
static int mouse_position[2];

int api_print(lua_State* L);
int api_button(lua_State* L);
int api_mouse_position(lua_State* L);
int api_draw_pixel(lua_State* L);
int api_draw_line(lua_State* L);
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

static const struct luaL_Reg draw_module_functions[] = {
    {"pixel", api_draw_pixel},
    {"line", api_draw_line},
    {"clear", api_clear_screen},
    {NULL, NULL}
};

int open_draw_module(lua_State* L) {
    luaL_newlib(L, draw_module_functions);
    return 1;
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

    // Set modules
    luaL_requiref(L, "draw", open_draw_module, 0);

    // Execute Lua script
    int result = luaL_dofile(L, "./assets/script.lua");

    if (result != LUA_OK) {
        const char* error_message = lua_tostring(L, -1);
        log_error(error_message);

        lua_pop(L, -1); // Error message
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
            if (event->key.code == KEYCODE_LEFT) {
                buttons[SCRIPT_BUTTON_LEFT] = true;
                return true;
            }
            else if (event->key.code == KEYCODE_RIGHT) {
                buttons[SCRIPT_BUTTON_RIGHT] = true;
                return true;
            }
            else if (event->key.code == KEYCODE_UP) {
                buttons[SCRIPT_BUTTON_UP] = true;
                return true;
            }
            else if (event->key.code == KEYCODE_DOWN) {
                buttons[SCRIPT_BUTTON_DOWN] = true;
                return true;
            }
            else if (event->key.code == KEYCODE_Z) {
                buttons[SCRIPT_BUTTON_A] = true;
                return true;
            }
            else if (event->key.code == KEYCODE_X) {
                buttons[SCRIPT_BUTTON_B] = true;
                return true;
            }

            break;

        case EVENT_KEYUP:
            if (event->key.code == KEYCODE_LEFT) {
                buttons[SCRIPT_BUTTON_LEFT] = false;
                return true;
            }
            else if (event->key.code == KEYCODE_RIGHT) {
                buttons[SCRIPT_BUTTON_RIGHT] = false;
                return true;
            }
            else if (event->key.code == KEYCODE_UP) {
                buttons[SCRIPT_BUTTON_UP] = false;
                return true;
            }
            else if (event->key.code == KEYCODE_DOWN) {
                buttons[SCRIPT_BUTTON_DOWN] = false;
                return true;
            }
            else if (event->key.code == KEYCODE_Z) {
                buttons[SCRIPT_BUTTON_A] = false;
                return true;
            }
            else if (event->key.code == KEYCODE_X) {
                buttons[SCRIPT_BUTTON_B] = false;
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
        lua_pop(L, -1); // nil or some other type
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

/**
 * @brief Prints given object to console.
 *
 * @param arg Object to print to console
 */
int api_print(lua_State* L) {
    const char* message = lua_tostring(L, -1);
    printf("%s\n", message);
    lua_pop(L, -1);

    return 0;
}

/**
 * @brief Get button state for given button id.
 *
 * @param id Button id to check
 * @return True if button is pressed, false otherwise
 */
int api_button(lua_State* L) {
    if (lua_isnumber(L, -1)) {
        int button_id = (int)lua_tonumber(L, -1);
        lua_pop(L, -1); // button id
        lua_pushboolean(L, buttons[button_id]);
    }
    else {
        lua_pushboolean(L, false);
    }

    return 1;
}

/**
 * @brief Get current mouse position.
 *
 * @return Mouse coords as x, y
 */
int api_mouse_position(lua_State* L) {
    lua_pushnumber(L, mouse_position[0]);
    lua_pushnumber(L, mouse_position[1]);
    return 2;
}

/**
 * @brief Draw a pixel at given position and color.
 *
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @param color Pixel color
 */
int api_draw_pixel(lua_State* L) {
    int x = (int)lua_tonumber(L, -3);
    int y = (int)lua_tonumber(L, -2);
    int color = (int)lua_tonumber(L, -1);

    lua_pop(L, -1); // color
    lua_pop(L, -1); // y
    lua_pop(L, -1); // x

    texture_t* render_texture = graphics_get_render_texture();
    texture_set_pixel(render_texture, x, y, color);

    return 0;
}

/**
 * @brief Draw a line between given position and color.
 *
 * @param x0 Start x-coordinate
 * @param y0 Start y-coordinate
 * @param x1 End x-coordinate
 * @param y1 End y-coordinate
 * @param color Line color
 */
int api_draw_line(lua_State* L) {
    int x0 = (int)lua_tonumber(L, -5);
    int y0 = (int)lua_tonumber(L, -4);
    int x1 = (int)lua_tonumber(L, -3);
    int y1 = (int)lua_tonumber(L, -2);
    int color = (int)lua_tonumber(L, -1);

    lua_pop(L, -1); // color
    lua_pop(L, -1); // y1
    lua_pop(L, -1); // x1
    lua_pop(L, -1); // y0
    lua_pop(L, -1); // x0

    texture_t* render_texture = graphics_get_render_texture();
    graphics_draw_line(render_texture, x0, y0, x1, y1, color);

    return 0;
}

/**
 * @brief Clear screen to given color.
 *
 * @param color Color to clear screen
 */
int api_clear_screen(lua_State* L) {
    int color = (int)lua_tonumber(L, -1);
    lua_pop(L, -1); // color

    texture_t* render_texture = graphics_get_render_texture();
    texture_clear(render_texture, color);

    return 0;
}