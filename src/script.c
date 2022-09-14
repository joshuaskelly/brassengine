#include <stdint.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "assets.h"
#include "event.h"
#include "graphics.h"
#include "log.h"
#include "script.h"
#include "renderers/draw.h"
#include "bindings/pico.h"

static lua_State* L = NULL;
static bool is_in_error_state = false;

static bool buttons[6];
static int mouse_position[2];

int api_print(lua_State* L);
int api_button(lua_State* L);
int api_mouse_position(lua_State* L);
int api_draw_pixel(lua_State* L);
int api_draw_line(lua_State* L);
int api_draw_rectangle(lua_State* L);
int api_draw_filled_rectangle(lua_State* L);
int api_draw_circle(lua_State* L);
int api_draw_filled_circle(lua_State* L);
int api_clear_screen(lua_State* L);
int api_set_palette_color(lua_State* L);
int api_set_clipping_rectangle(lua_State* L);
int api_test_blit(lua_State* L);

/**
 * Add a global function to Lua VM
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
    {"rectangle", api_draw_rectangle},
    {"filled_rectangle", api_draw_filled_rectangle},
    {"circle", api_draw_circle},
    {"filled_circle", api_draw_filled_circle},
    {"clear", api_clear_screen},
    {"test_blit", api_test_blit},
    {NULL, NULL}
};

int open_draw_module(lua_State* L) {
    luaL_newlib(L, draw_module_functions);
    return 1;
}

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

    // Set globals
    add_global_function(L, api_print, "print");
    add_global_function(L, api_button, "button");
    add_global_function(L, api_mouse_position, "mouse_position");
    add_global_function(L, api_set_palette_color, "palette");
    add_global_function(L, api_set_clipping_rectangle, "clip");

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

/**
 * Draw a pixel at given position and color.
 *
 * @param x Pixel x-coordinate
 * @param y Pixel y-coordinate
 * @param color Pixel color
 */
int api_draw_pixel(lua_State* L) {
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
int api_draw_line(lua_State* L) {
    int x0 = (int)lua_tonumber(L, -5);
    int y0 = (int)lua_tonumber(L, -4);
    int x1 = (int)lua_tonumber(L, -3);
    int y1 = (int)lua_tonumber(L, -2);
    int color = (int)lua_tonumber(L, -1);

    lua_pop(L, -1);

    draw_line(x0, y0, x1, y1, color);

    return 0;
}

int api_draw_rectangle(lua_State* L) {
    int x = (int)lua_tonumber(L, -5);
    int y = (int)lua_tonumber(L, -4);
    int width = (int)lua_tonumber(L, -3);
    int height = (int)lua_tonumber(L, -2);
    int color = (int)lua_tonumber(L, -1);

    lua_pop(L, -1);

    draw_rectangle(x, y, width, height, color);

    return 0;
}

int api_draw_filled_rectangle(lua_State* L) {
    int x = (int)lua_tonumber(L, -5);
    int y = (int)lua_tonumber(L, -4);
    int width = (int)lua_tonumber(L, -3);
    int height = (int)lua_tonumber(L, -2);
    int color = (int)lua_tonumber(L, -1);

    lua_pop(L, -1);

    draw_filled_rectangle(x, y, width, height, color);

    return 0;
}

int api_draw_circle(lua_State* L) {
    int x = (int)lua_tonumber(L, -4);
    int y = (int)lua_tonumber(L, -3);
    int radius = (int)lua_tonumber(L, -2);
    int color = (int)lua_tonumber(L, -1);

    lua_pop(L, -1);

    draw_circle(x, y, radius, color);

    return 0;
}

int api_draw_filled_circle(lua_State* L) {
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
int api_clear_screen(lua_State* L) {
    int color = (int)lua_tonumber(L, -1);

    lua_pop(L, -1);

    texture_t* render_texture = graphics_get_render_texture();
    graphics_texture_clear(render_texture, color);

    return 0;
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

int api_test_blit(lua_State* L) {
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
