/**
 * Module for mode7 renderer
 *
 * @module mode7
 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "matrix3.h"
#include "texture.h"
#include "vector3.h"

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

/**
 * @type Renderer
 */

/**
 * Creates a mode7 renderer object.
 * @function Renderer:new
 * @tparam ?texture.texture render_texture Optional texture to render to. If none given, will default to global render texture.
 * @treturn Renderer
 */
static int modules_mode7_renderer_new(lua_State* L) {
    return lua_newmode7renderer(L);
}

/**
 * Clears color buffer.
 * @function Renderer:clear
 * @tparam ?integer color Color to fill buffer with.
 */
static int modules_mode7_renderer_clear(lua_State* L) {
    mode7_renderer_t* renderer = luaL_checkmode7renderer(L, 1);
    color_t color = (color_t)luaL_optnumber(L, 2, 0);
    mode7_renderer_clear_color(renderer, color);

    return 0;
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

    // If given userdata, get it's metatable
    if (lua_isuserdata(L, index)) {
        lua_getmetatable(L, index);
    }

    if (lua_istable(L, -1)) {
        int type = lua_getfield(L, -1, "__call");
        // Remove whatever was put on the stack, we only care about the type
        lua_settop(L, base);

        return type == LUA_TFUNCTION;
    }

    return false;
}

/**
 * Renders given texture.
 * @function render
 * @tparam texture.texture texture Texture to render
 * @tparam function(integer):nil callback Horizontal scanline callback. Given the scanline as integer, and does not return a value.
 */
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
        else if (lua_isuserdata(L, 3)) {
            if (lua_getmetatable(L, 3)) {
                lua_getfield(L, -1, "__call");

                // Remove metatable
                lua_remove(L, -2);

                is_method = true;
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

/**
 * Access renderer's features. If just the feature name is provided, the value of
 * that feature will be returned. If a value is provided, the feature will be set to that value.
 *
 * **Features:**
 *
 *  * <span class="parameter">'matrix'</span> (@{matrix3}) Matrix used to transform scanline.
 *  * <span class="parameter">'wrapmode'</span> (@{string}) Mode defining how the texture is wrapped. One of: 'NONE', 'REPEAT', or 'CLAMP'
 *
 * @function Renderer:feature
 * @tparam string name Feature name.
 * @tparam ?any value Value to set feature to.
 */
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
    {"clear", modules_mode7_renderer_clear},
    {"render", modules_mode7_renderer_render},
    {"feature", modules_mode7_renderer_feature},
    {NULL, NULL}
};

static const struct luaL_Reg modules_mode7_renderer_meta_functions[] = {
    {"__index", modules_mode7_renderer_meta_index},
    {NULL, NULL}
};

static mode7_camera_t* luaL_checkmode7camera(lua_State* L, int index) {
    mode7_camera_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    handle = (mode7_camera_t**)luaL_checkudata(L, index, "mode7_camera");

    if (!handle) {
        luaL_typeerror(L, index, "mode7_camera");
    }

    return *handle;
}

static int lua_newmode7camera(lua_State* L) {
    mode7_renderer_t* renderer = luaL_checkmode7renderer(L, 2);
    mode7_camera_t** handle = (mode7_camera_t**)lua_newuserdata(L, sizeof(mode7_camera_t*));
    *handle = mode7_camera_new(renderer);
    luaL_setmetatable(L, "mode7_camera");

    return 1;
}

static int modules_mode7_camera_new(lua_State* L) {
    return lua_newmode7camera(L);
}

static int modules_mode7_camera_call(lua_State* L) {
    mode7_camera_t* camera = luaL_checkmode7camera(L, 1);
    int scanline = luaL_checkinteger(L, 2);
    mode7_camera_call(camera, scanline);

    return 0;
}

static int modules_mode7_camera_meta_index(lua_State* L) {
    mode7_camera_t* camera = luaL_checkmode7camera(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_settop(L, 0);

    if (strcmp(key, "position") == 0) {
        lua_pushvector3(L, camera->position);
    }
    else if (strcmp(key, "pitch") == 0) {
        lua_pushnumber(L, camera->pitch);
    }
    else if (strcmp(key, "yaw") == 0) {
        lua_pushnumber(L, camera->yaw);
    }
    else if (strcmp(key, "fov") == 0) {
        lua_pushnumber(L, camera->fov);
    }
    else if (strcmp(key, "near") == 0) {
        lua_pushnumber(L, camera->near);
    }
    else if (strcmp(key, "far") == 0) {
        lua_pushnumber(L, camera->far);
    }
    else {
        luaL_requiref(L, "mode7", NULL, false);
        lua_getfield(L, -1, "Camera");
        if (lua_type(L, -1) == LUA_TTABLE) {
            lua_getfield(L, -1, key);
        }
        else {
            lua_pushnil(L);
        }
    }

    return 1;
}

static int modules_mode7_camera_meta_newindex(lua_State* L) {
    mode7_camera_t* camera = luaL_checkmode7camera(L, 1);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "position") == 0) {
        mfloat_t* position = luaL_checkvector3(L, 3);
        vec3_assign(camera->position, position);
    }
    else if (strcmp(key, "pitch") == 0) {
        float pitch = luaL_checknumber(L, 3);
        camera->pitch = pitch;
    }
    else if (strcmp(key, "yaw") == 0) {
        float yaw = luaL_checknumber(L, 3);
        camera->yaw = yaw;
    }
    else if (strcmp(key, "fov") == 0) {
        float fov = luaL_checknumber(L, 3);
        camera->fov = fov;
    }
    else if (strcmp(key, "near") == 0) {
        float near = luaL_checknumber(L, 3);
        camera->near = near;
    }
    else if (strcmp(key, "far") == 0) {
        float far = luaL_checknumber(L, 3);
        camera->far = far;
    }
    else {
        luaL_error(L, "attempt to index a mode7_camera value");
    }

    lua_pop(L, -1);

    return 0;
}

static const struct luaL_Reg modules_mode7_camera_functions[] = {
    {"new", modules_mode7_camera_new},
    {NULL, NULL}
};

static const struct luaL_Reg modules_mode7_camera_meta_functions[] = {
    {"__index", modules_mode7_camera_meta_index},
    {"__newindex", modules_mode7_camera_meta_newindex},
    {"__call", modules_mode7_camera_call},
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

    lua_pushstring(L, "Camera");
    luaL_newlib(L, modules_mode7_camera_functions);
    lua_settable(L, -3);

    luaL_newmetatable(L, "mode7_camera");
    luaL_setfuncs(L, modules_mode7_camera_meta_functions, 0);
    lua_pop(L, 1);

    return 1;
}
