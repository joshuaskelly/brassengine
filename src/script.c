#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "assets.h"
#include "event.h"
#include "files.h"
#include "graphics.h"
#include "input.h"
#include "log.h"
#include "platform.h"
#include "script.h"
#include "time.h"

#include "modules/assets.h"
#include "modules/draw.h"
#include "modules/float_array.h"
#include "modules/gif.h"
#include "modules/globals.h"
#include "modules/graphics.h"
#include "modules/int_array.h"
#include "modules/json.h"
#include "modules/keyboard.h"
#include "modules/matrix2.h"
#include "modules/matrix3.h"
#include "modules/matrix4.h"
#include "modules/math_extensions.h"
#include "modules/mouse.h"
#include "modules/quaternion.h"
#include "modules/raycaster.h"
#include "modules/sound.h"
#include "modules/statistics.h"
#include "modules/texture.h"
#include "modules/vector2.h"
#include "modules/vector3.h"
#include "modules/vector4.h"

static lua_State* L = NULL;
static bool is_in_error_state = false;
static double update_time;
static double draw_time;

#define MAX_SUGGESTIONS 128

static int lua_package_searcher(lua_State* L);
static int io_open(lua_State* L);
static int call(lua_State* L, int narg, int nresults);

static const luaL_Reg modules[] = {
    {"assets", luaopen_assets},
    {"draw", luaopen_draw},
    {"json", luaopen_json},
    {"floatarray", luaopen_floatarray},
    {"gif", luaopen_gif},
    {"graphics", luaopen_graphics},
    {"intarray", luaopen_intarray},
    {"texture", luaopen_texture},
    {"keyboard", luaopen_keyboard},
    {"matrix2", luaopen_matrix2},
    {"matrix3", luaopen_matrix3},
    {"matrix4", luaopen_matrix4},
    {"mouse", luaopen_mouse},
    {"quaternion", luaopen_quaternion},
    {"raycaster", luaopen_raycaster},
    {"sound", luaopen_sound},
    {"statistics", luaopen_statistics},
    {"vector2", luaopen_vector2},
    {"vector3", luaopen_vector3},
    {"vector4", luaopen_vector4},
    {NULL, NULL}
};

/**
 * Opens all engine modules.
 *
 * @param L Lua VM
 */
static void luaL_openenginemodules(lua_State* L) {
    const luaL_Reg *module;
    for (module = modules; module->func; module++) {
        luaL_requiref(L, module->name, module->func, 0);
        lua_pop(L, 1);
    }

    luaopen_mathextensions(L);
}

/**
 * Executes given string as a Lua chunk. Will set error state if execution
 * fails.
 *
 * @param L Lua VM
 * @param str Lua code to execute.
 * @return true if sucessful. false otherwise.
 */
static bool do_string(lua_State*L, char* str) {
    int status = luaL_dostring(L, str);

    if (status != LUA_OK) {
        const char* error_message = lua_tostring(L, -1);
        log_error(error_message);
        is_in_error_state = true;

        lua_pop(L, -1);
        return false;
    }

    return true;
}

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
    if (!do_string(L, "table.insert(package.searchers, 2, _lua_package_searcher)")) return;

    // Override default io.open behavior
    lua_register(L, "_io_open", io_open);
    if (!do_string(L, "io.open = _io_open")) return;

    char buffer[1024];
    const char* version = LUA_VERSION_MAJOR "." LUA_VERSION_MINOR;

    // Set Lua package.path field.
    sprintf(
        buffer,
        "package.path = \""
        "./%s/lua_modules/share/lua/%s/?.lua;"
        "./%s/lua_modules/share/lua/%s/?/init.lua;"
        "./%s/?.lua;"
        "./%s/?/init.lua"
        "\"",
        assets_directory,
        version,
        assets_directory,
        version,
        assets_directory,
        assets_directory
    );

    if (!do_string(L, buffer)) return;

#if defined(_WIN32)
    const char* extension = "dll";
#else
    const char* extension = "so";
#endif

    // Set Lua package.cpath field.
    sprintf(
        buffer,
        "package.cpath = \""
        "./%s/lua_modules/lib/lua/%s/?.%s;"
        "./%s/?.%s"
        "\"",
        assets_directory,
        version,
        extension,
        assets_directory,
        extension
    );

    if (!do_string(L, buffer)) return;

    // Load globals
    luaL_openglobals(L);

    // Load modules
    luaL_openenginemodules(L);

    // Load platform specific module
    platform_open_module(L);

   // Execute Lua script
    const char* main = assets_script_get("main.lua");
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
    call_global_lua_function(L, "_init");
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
    char buffer[strlen(script)+9];
    sprintf(buffer, "return %s;", script);

    // Try to evaluate wrapped in a return statement.
    int status = luaL_loadbuffer(L, buffer, strlen(buffer), "=console");

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

static int compare_strings(const void* a, const void* b) {
    return strcmp(*(const char**)a, *(const char**)b);
}

void script_complete(char* expression) {
    // TODO Potentially look for the equals symbol for the root? E.g.
    // platform = require("platform")
    // TODO Trim leading white space?

    /*
     * Attempt to decompose given expression into a root and a partial. The
     * root is used to determine which table to inspect. If no root is found,
     * then use global table and treat entire expression as partial.
     *
     * Expression with root and partial:
     *
     *   module.submodule.my_functio
     *   |-----root-----| |-partial-|
     *
     *
     * Expression with only partial:
     *
     *   collectgarbag
     *   |--partial--|
     */

    char root[2048];
    char* partial = expression;

    char* last_dot = strrchr(expression, '.');
    size_t dot_position = last_dot - expression + 1;
    int suggestion_count = 0;

    int top = lua_gettop(L);

    // No root found, use global table.
    if (last_dot == NULL) {
        dot_position = 0;
        lua_getglobal(L, LUA_GNAME);
        if (lua_type(L, 1) != LUA_TTABLE) goto done;
    }
    // Evaluate the root. If the result is a table, use it.
    else {
        // Set root + partial
        strncpy(root, expression, dot_position - 1);
        root[dot_position - 1] = '\0';
        partial = expression + dot_position;

        // Load expression as buffer
        char buffer[2048];
        sprintf(buffer, "return %s;", root);
        buffer[strlen(buffer)] = '\0';
        int status = luaL_loadbuffer(L, buffer, strlen(buffer), NULL);
        if (status != LUA_OK) goto done;

        // Evaluate buffer
        status = lua_pcall(L, 0, 1, 0);
        if (status != LUA_OK) goto done;

        // Ensure result is what we expect
        if (lua_type(L, 1) != LUA_TTABLE) goto done;
    }

    const char* suggestions[MAX_SUGGESTIONS];

    // Iterate table key/value pairs
    int base = lua_gettop(L);
    lua_pushnil(L);
    size_t size = strlen(partial);
    while (lua_next(L, base) != 0) {
        const char* key = luaL_checkstring(L, -2);
        lua_pop(L, 1);

        if (strncmp(partial, key, size) == 0) {
            suggestions[suggestion_count++] = key;
        }
    }

    // Sort results
    qsort(suggestions, suggestion_count, sizeof(char*), compare_strings);

    // Only one valid result, complete the expression
    if (suggestion_count == 1) {
        const char* suggestion = suggestions[0];
        int length = strlen(suggestion);
        strncpy(expression + dot_position, suggestions[0], length);
        expression[dot_position + length] = '\0';
    }
    // Otherwise complete as much as possible
    else {
        char* sep = dot_position ? "." : "";
        for (int i = 0; i < suggestion_count; i++) {
            log_info("%s%s%s", root, sep, suggestions[i]);
        }

        // TODO Complete as much of the command as possible
    }

done:
    // Cleanup Lua VM state
    lua_settop(L, top);
}

double script_update_time_get(void) {
    return update_time;
}

double script_draw_time_get(void) {
    return draw_time;
}

void script_update(void) {
    double start = time_millis_get();
    call_global_lua_function(L, "_update");
    update_time = time_millis_get() - start;
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
    const char* script = assets_script_get(filename);

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

    p->f = files_open(filename, mode);

    return (p->f == NULL) ? luaL_fileresult(L, 0, filename) : 1;
}
