#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "assets.h"
#include "event.h"
#include "graphics.h"
#include "log.h"
#include "script.h"
#include "bindings/draw.h"
#include "bindings/pico.h"

static lua_State* L = NULL;
static bool is_in_error_state = false;

static bool buttons[6];
static int mouse_position[2];

int api_print(lua_State* L);
int api_button(lua_State* L);
int api_mouse_position(lua_State* L);
int api_set_palette_color(lua_State* L);
int api_set_clipping_rectangle(lua_State* L);
int lua_package_searcher(lua_State* L);

/**
 * Create and configure Lua VM.
 */
void init_lua_vm(void) {
    // Create Lua VM
    L = luaL_newstate();
    is_in_error_state = false;

    if (!L) {
        log_fatal("Error creating Lua VM");
    }

    luaL_openlibs(L);

    // Add package searcher
    lua_register(L, "_lua_package_searcher", lua_package_searcher);
    luaL_dostring(L, "table.insert(package.searchers, 2, _lua_package_searcher)");

    // Set globals
    lua_register(L, "print", api_print);
    lua_register(L, "button", api_button);
    lua_register(L, "mouse_position", api_mouse_position);
    lua_register(L, "palette", api_set_palette_color);
    lua_register(L, "clip", api_set_clipping_rectangle);

    // Set modules
    luaL_requiref(L, "draw", open_draw_module, 0);
    luaL_requiref(L, "apis/pico", open_pico_module, 0);

    // Execute Lua script
    int result = luaL_dostring(L, assets_get_script("main.lua"));

    if (result != LUA_OK) {
        const char* error_message = lua_tostring(L, -1);
        log_error(error_message);
        is_in_error_state = true;

        lua_pop(L, -1);
    }
}

/**
 * Error message handler.
 *
 * @param L Lua VM
 * @return int 1. A traceback or message will be pushed to the stack.
 */
static int message_handler (lua_State *L) {
    const char *msg = lua_tostring(L, 1);

    if (msg == NULL) {
        // Does it have a metamethod that can produce a string?
        if (luaL_callmeta(L, 1, "__tostring") && lua_type(L, -1) == LUA_TSTRING) {
            return 1;
        }
        else {
            msg = lua_pushfstring(L, "(error object is a %s value)", luaL_typename(L, 1));
        }
    }

    // Append a standard traceback.
    luaL_traceback(L, L, msg, 1);

    return 1;
}

/**
 * Call a globally defined Lua function. The function
 * must take no arguments and return no values.
 *
 * @param L Lua VM
 * @param function_name Name of the function to call
 */
void call_global_lua_function(lua_State* L, const char* function_name) {
    if (is_in_error_state) return;

    int base = lua_gettop(L);

    // Attempt to find the global object
    lua_getglobal(L, function_name);

    // Invoke function
    if (lua_isfunction(L, -1)) {
        // Set message handler
        lua_pushcfunction(L, message_handler);
        // Move message handler under function and args
        lua_insert(L, base);

        if (lua_pcall(L, 0, 0, base)) {
            // Handle traceback if we get one.
            const char* message = lua_tostring(L, -1);
            log_error(message);
            is_in_error_state = true;
        }

        // Remove message handler
        lua_remove(L, base);
    }
}

void script_init(void) {
    log_info("script init (" LUA_RELEASE ")");
    init_lua_vm();
}

void script_destroy(void) {
    lua_close(L);
}

void script_reload(void) {
    log_info("script reload");
    assets_reload();

    init_lua_vm();
    call_global_lua_function(L, "_init");
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
            if (event->key.code == KEYCODE_F5) {
                script_reload();
                return true;
            }
            else if (event->key.code == KEYCODE_LEFT) {
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
 * Prints given object to console.
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
 * Get button state for given button id.
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
 * Get current mouse position.
 *
 * @return Mouse coords as x, y
 */
int api_mouse_position(lua_State* L) {
    lua_pushnumber(L, mouse_position[0]);
    lua_pushnumber(L, mouse_position[1]);

    return 2;
}

int api_set_palette_color(lua_State* L) {
    int index = (int)lua_tonumber(L, -4);
    int r = (int)lua_tonumber(L, -3) & 0xFF;
    int g = (int)lua_tonumber(L, -2) & 0xFF;
    int b = (int)lua_tonumber(L, -1) & 0xFF;
    int a = 0xFF;

    lua_pop(L, -1);

    uint32_t color = a << 24 | b << 16 | g << 8 | r;

    uint32_t* palette = NULL;
    palette = graphics_palette_get();
    palette[index] = color;

    return 0;
}

int api_set_clipping_rectangle(lua_State* L) {
    int x = (int)lua_tonumber(L, -4);
    int y = (int)lua_tonumber(L, -3);
    int width = (int)lua_tonumber(L, -2);
    int height = (int)lua_tonumber(L, -1);

    lua_pop(L, -1);

    graphics_set_clipping_rectangle(x, y, width, height);

    return 0;
}

/**
 * @brief Module searcher function that loads scripts from assets module.
 *
 * @param name Module name to search for
 */
int lua_package_searcher(lua_State* L) {
    const char* module_name = luaL_checkstring(L, 1);

    // Append .lua to module name
    char filename[strlen(module_name) + 5];
    memset(filename, 0, strlen(module_name) + 5);
    strcat(filename, module_name);
    strcat(filename, ".lua\0");

    // Look for script asset
    const char* script = assets_get_script(filename);

    if (script) {
        // We found a script asset, remove the module name from the stack.
        lua_pop(L, 1);

        // Load script.
        int result = luaL_loadbuffer(L, script, strlen(script), module_name);
        if (result != LUA_OK) {
            message_handler(L);
        }
    }

    return 1;
}
