#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "matrix3.h"
#include "texture.h"

#include "../log.h"
#include "../script.h"
#include "../renderers/mode7.h"

static mode7_renderer_t* luaL_checkmode7renderer(lua_State* L, int index) {
    mode7_renderer_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    handle = (mode7_renderer_t**)luaL_checkudata(L, index, "mode7_renderer");

    if (!handle) {
        luaL_typeerror(L, index, "mode7_renderer");
    }

    return *handle;
}

static int lua_newmode7renderer(lua_State* L) {
    texture_t* render_texture = luaL_opttexture(L, 2, graphics_render_texture_get());
    mode7_renderer_t** handle = (mode7_renderer_t**)lua_newuserdata(L, sizeof(mode7_renderer_t*));
    *handle = mode7_renderer_new(render_texture);
    luaL_setmetatable(L, "mode7_renderer");

    return 1;
}

static int modules_mode7_renderer_new(lua_State* L) {
    return lua_newmode7renderer(L);
}

static lua_State* LL;
static int table_reference = 0;
static int callback_reference = 0;

static void callback(int y) {
    lua_State* L = LL;

    // Get Lua callback function
    lua_rawgeti(L, LUA_REGISTRYINDEX, callback_reference);

    // Get table reference if callback is a method
    if (table_reference > 0) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, table_reference);
    }

    lua_pushinteger(L, y);

    if (lua_pcall(L, 1, 0, 0) != 0) {
        // Handle traceback if we get one.
        const char* message = lua_tostring(L, -1);
        log_error(message);

        lua_pop(L, -1);

        script_error_state_set(true);
    }
}

/**
 * Returns true if value at given index is a function (C or Lua) or a table
 * with the __call metamethod, and false otherwise.
 */
static bool lua_iscallable(lua_State*L, int index) {
    if (lua_isfunction(L, index)) return true;

    int base = lua_gettop(L);

    if (lua_istable(L, index)) {
        int type = lua_getfield(L, index, "__call");
        // Remove whatever was put on the stack, we only care about the type
        lua_settop(L, base);

        return type == LUA_TFUNCTION;
    }

    return false;
}

static int modules_mode7_renderer_render(lua_State* L) {
    mode7_renderer_t* renderer = luaL_checkmode7renderer(L, 1);
    texture_t* texture = luaL_checktexture(L, 2);

    bool is_method = false;

    if (lua_iscallable(L, 3)) {
        if (lua_istable(L, 3)) {
            // Put the table's __call function on stack
            lua_getfield(L, 3, "__call");
            lua_pushvalue(L, -1);

            // If function takes two params, assume it is a method
            lua_Debug ar;
            lua_getinfo(L, ">nu", &ar);
            is_method = ar.nparams > 1;

            // Remove the table if not a method
            if (!is_method) {
                lua_remove(L, 3);
            }
        }
    }
    else {
        luaL_typeerror(L, 3, "function");
    }

    // Remove renderer and texture so the callback is on top of the stack
    lua_remove(L, 1);
    lua_remove(L, 1);

    // Get a reference to the Lua callback function
    callback_reference = luaL_ref(L, LUA_REGISTRYINDEX);

    // Get a reference to the table if a method
    if (is_method) {
        table_reference = luaL_ref(L, LUA_REGISTRYINDEX);
    }

    // Cache pointer to Lua VM
    LL = L;

    mode7_renderer_render(renderer, texture, callback);

    // Release reference to Lua callback function
    luaL_unref(L, LUA_REGISTRYINDEX, callback_reference);
    callback_reference = 0;

    // Release reference to table
    if (is_method) {
        luaL_unref(L, LUA_REGISTRYINDEX, table_reference);
        table_reference = 0;
    }

    return 0;
}

static int modules_mode7_renderer_feature(lua_State* L) {
    mode7_renderer_t* renderer = luaL_checkmode7renderer(L, 1);
    const char* key = luaL_checkstring(L, 2);
    bool is_setter = lua_gettop(L) > 2;

    if (strcmp(key, "matrix") == 0) {
        if (is_setter) {
            mfloat_t* m = luaL_checkmatrix3(L, 3);
            mat3_assign(renderer->matrix, m);

            return 0;
        }

        lua_pushmatrix3(L, renderer->matrix);

        return 1;
    }
    else if (strcmp(key, "wrapmode") == 0) {
        if (is_setter) {
            const char* mode = luaL_checkstring(L, 3);

            if (strcmp(mode, "NONE") == 0) {
                renderer->features.wrap_mode = WRAP_NONE;
            }
            else if (strcmp(mode, "REPEAT") == 0) {
                renderer->features.wrap_mode = WRAP_REPEAT;
            }
            else if (strcmp(mode, "CLAMP") == 0) {
                renderer->features.wrap_mode = WRAP_CLAMP;
            }

            return 0;
        }

        switch (renderer->features.wrap_mode) {
            case WRAP_NONE:
                lua_pushstring(L, "NONE");
                break;

            case WRAP_REPEAT:
                lua_pushstring(L, "REPEAT");
                break;

            case WRAP_CLAMP:
                lua_pushstring(L, "CLAMP");
                break;
        }

        return 1;
    }

    return 0;
}

static int modules_mode7_renderer_meta_index(lua_State* L) {
    luaL_checkmode7renderer(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_settop(L, 0);

    luaL_requiref(L, "mode7", NULL, false);
    lua_getfield(L, -1, "Renderer");
    if (lua_type(L, -1) == LUA_TTABLE) {
        lua_getfield(L, -1, key);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static const struct luaL_Reg modules_mode7_renderer_functions[] = {
    {"new", modules_mode7_renderer_new},
    {"render", modules_mode7_renderer_render},
    {"feature", modules_mode7_renderer_feature},
    {NULL, NULL}
};

static const struct luaL_Reg modules_mode7_renderer_meta_functions[] = {
    {"__index", modules_mode7_renderer_meta_index},
    {NULL, NULL}
};

int luaopen_mode7(lua_State* L) {
    lua_newtable(L);

    lua_pushstring(L, "Renderer");
    luaL_newlib(L, modules_mode7_renderer_functions);
    lua_settable(L, -3);

    luaL_newmetatable(L, "mode7_renderer");
    luaL_setfuncs(L, modules_mode7_renderer_meta_functions, 0);
    lua_pop(L, 1);

    return 1;
}
