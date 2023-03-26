#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "assets.h"
#include "event.h"
#include "graphics.h"
#include "input.h"
#include "log.h"
#include "script.h"
#include "time.h"

#include "modules/assets.h"
#include "modules/display.h"
#include "modules/draw.h"
#include "modules/float_array.h"
#include "modules/globals.h"
#include "modules/graphics.h"
#include "modules/json.h"
#include "modules/keyboard.h"
#include "modules/mesh.h"
#include "modules/mouse.h"
#include "modules/raycaster.h"
#include "modules/sound.h"
#include "modules/statistics.h"
#include "modules/texture.h"
#include "modules/vector2.h"

static lua_State* L = NULL;
static bool is_in_error_state = false;
static double update_time;
static double draw_time;

static int lua_package_searcher(lua_State* L);
static int io_open(lua_State* L);
static int call(lua_State* L, int narg, int nresults);

/**
 * Create and configure Lua VM.
 */
static void init_lua_vm(void) {
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

    // Override default io.open behavior
    lua_register(L, "_io_open", io_open);
    luaL_dostring(L, "io.open = _io_open");

    // Set globals
    luaL_openglobals(L);

    // Set modules
    luaL_requiref(L, "assets", luaopen_assets, 0);
    luaL_requiref(L, "display", luaopen_display, 0);
    luaL_requiref(L, "draw", luaopen_draw, 0);
    luaL_requiref(L, "floatarray", luaopen_floatarray, 0);
    luaL_requiref(L, "json", luaopen_json, 0);
    luaL_requiref(L, "graphics", luaopen_graphics, 0);
    luaL_requiref(L, "graphics.texture", luaopen_texture, 0);
    luaL_requiref(L, "input.keyboard", luaopen_keyboard, 0);
    luaL_requiref(L, "input.mouse", luaopen_mouse, 0);
    luaL_requiref(L, "mesh", luaopen_mesh, 0);
    luaL_requiref(L, "raycaster", luaopen_raycaster, 0);
    luaL_requiref(L, "sound", luaopen_sound, 0);
    luaL_requiref(L, "statistics", luaopen_statistics, 0);
    luaL_requiref(L, "vector2", luaopen_vector2, 0);
    lua_pop(L, -1);

   // Execute Lua script
    const char* main = assets_get_script("main.lua");
    if (!main) {
        log_error("Failed to load main.lua file.");
        return;
    }

    int status = luaL_loadbuffer(L, main, strlen(main), "=main.lua");

    if (status != LUA_OK) {
        const char* error_message = lua_tostring(L, -1);
        log_error(error_message);
        is_in_error_state = true;

        lua_pop(L, -1);
        return;
    }

    int result = call(L, 0, LUA_MULTRET);
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
static void call_global_lua_function(lua_State* L, const char* function_name) {
    if (is_in_error_state) return;

    // Attempt to find the global object
    lua_getglobal(L, function_name);

    int base = lua_gettop(L);

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

            lua_pop(L, -1);
            return;
        }

        // Remove message handler
        lua_remove(L, base);
    }
    else {
        lua_pop(L, 1);
    }
}

/**
 * Call function on top of stack.
 *
 * @param L Lua VM
 * @param narg Number of arguments on stack
 * @param nresults Number of results to return, unless nresults is LUA_MULTRET.
 * @return int Status of call
 */
static int call(lua_State* L, int narg, int nresults) {
    int base = lua_gettop(L);
    lua_pushcfunction(L, message_handler);
    lua_insert(L, base);
    int status = lua_pcall(L, narg, nresults, base);
    lua_remove(L, base);

    return status;
}

void script_init(void) {
    log_info("script init (" LUA_RELEASE ")");
    init_lua_vm();
}

void script_destroy(void) {
    lua_close(L);
}

void script_reload(void) {
    lua_close(L);

    init_lua_vm();
    call_global_lua_function(L, "_init");
}

int script_evaluate(const char* script) {
    char r[strlen(script)+9];
    sprintf(r, "return %s;", script);

    // Try to evaluate wrapped in a return statement.
    int status = luaL_loadbuffer(L, r, strlen(r), "=console");

    if (status != LUA_OK) {
        // Remove previous error message.
        lua_pop(L, 1);

        // Try to evaluate as is.
        status = luaL_loadbuffer(L, script, strlen(script), "=console");
    }

    if (status == LUA_OK) {
        int result = call(L, 0, LUA_MULTRET);

        // Print any values on the stack
        int n = lua_gettop(L);
        if (n > 0) {
            luaL_checkstack(L, LUA_MINSTACK, "too many results to print");
            lua_getglobal(L, "print");
            lua_insert(L, 1);
            if (lua_pcall(L, n, 0, 0) != LUA_OK) {
                log_error("error calling 'print'");
            }
        }

        return result;
    }

    return status;
}

double script_update_time_get(void) {
    return update_time;
}

double script_draw_time_get(void) {
    return draw_time;
}

bool script_handle_event(event_t* event) {
    return false;
}

void script_update(void) {
    double start = time_millis_get();
    call_global_lua_function(L, "_update");
    update_time = time_millis_get() - start;
}

void script_setup(void) {
    call_global_lua_function(L, "_init");
}

void script_draw(void) {
    double start = time_millis_get();
    call_global_lua_function(L, "_draw");
    draw_time = time_millis_get() - start;
}

/**
 * @brief Module searcher function that loads scripts from assets module.
 *
 * @param name Module name to search for
 */
static int lua_package_searcher(lua_State* L) {
    const char* module_name = luaL_checkstring(L, 1);

    // Copy module name
    char filename[strlen(module_name) + 5];
    memset(filename, 0, strlen(module_name) + 5);
    strcat(filename, module_name);

    // Replace dot with path separator
    char* next_dot = strchr(filename, '.');
    while (next_dot) {
        *next_dot = '/';
        next_dot = strchr(next_dot, '.');
    }

    // Append .lua to module name
    strcat(filename, ".lua\0");

    // Look for script asset
    const char* script = assets_get_script(filename);

    if (script) {
        // We found a script asset, remove the module name from the stack.
        lua_pop(L, 1);

        // Load script.
        size_t size = strlen(module_name) + 6;
        char name[size];
        snprintf(name, size, "=%s.lua", module_name);
        int result = luaL_loadbuffer(L, script, strlen(script), name);
        if (result != LUA_OK) {
            message_handler(L);
        }
    }

    return 1;
}

static luaL_Stream *newprefile (lua_State *L) {
  luaL_Stream *p = (luaL_Stream *)lua_newuserdatauv(L, sizeof(luaL_Stream), 0);
  p->closef = NULL;  /* mark file handle as 'closed' */
  luaL_setmetatable(L, LUA_FILEHANDLE);
  return p;
}

static int io_fclose (lua_State *L) {
  luaL_Stream *p =  (luaL_Stream *)luaL_checkudata(L, 1, LUA_FILEHANDLE);
  int res = fclose(p->f);
  return luaL_fileresult(L, (res == 0), NULL);
}

static luaL_Stream *newfile (lua_State *L) {
  luaL_Stream *p = newprefile(L);
  p->f = NULL;
  p->closef = &io_fclose;
  return p;
}

static int l_checkmode (const char *mode) {
  return (*mode != '\0' && strchr("rwa", *(mode++)) != NULL &&
         (*mode != '+' || ((void)(++mode), 1)) &&  /* skip if char is '+' */
         (strspn(mode, "b") == strlen(mode)));  /* check extensions */
}

/**
 * @brief Opens a file inside asset directory or zip.
 *
 * This was largely lifted from liolib.c
 *
 * @param filename Name of file
 * @param mode File access mode
 * @return File handle
 */
static int io_open(lua_State* L) {
    const char *filename = luaL_checkstring(L, 1);
    const char *mode = luaL_optstring(L, 2, "r");
    luaL_Stream *p = newfile(L);
    const char *md = mode;  /* to traverse/check mode */
    luaL_argcheck(L, l_checkmode(md), 2, "invalid mode");

    p->f = assets_open_file(filename, mode);

    return (p->f == NULL) ? luaL_fileresult(L, 0, filename) : 1;
}
