/**
 * Module for raycaster renderer
 *
 * @usage
 * raycaster = require('raycaster')
 *
 * -- Tile palette for rendering the map
 * tiles = {
 *    [1] = assets.get_texture('stonewall.gif)
 * }
 *
 * -- Camera info
 * position = vector2.new()
 * direction = vector2.new(0, 1)
 * fov = 90
 *
 * -- Sprite
 * sprite = assets.get_texture('sprite.gif')
 * sprite_position = vector2.new()
 *
 * -- Map
 * map = raycaster.Map:new()
 * map.walls = assets.get_texture('walls.gif')
 *
 * renderer = raycaster.Renderer:new()
 * renderer:feature('fog', 16)
 *
 * -- Render loop
 * renderer:clear('all')
 * renderer:camera(position, direction, fov)
 * renderer:render(map, tiles)
 * renderer:render(sprite, sprite_position)
 *
 * @module raycaster
 */

#include <stdlib.h>
#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "raycaster.h"
#include "texture.h"
#include "vector2.h"

#include "../renderers/raycaster.h"

static raycaster_renderer_t* luaL_checkrayrenderer(lua_State* L, int index) {
    raycaster_renderer_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    handle = (raycaster_renderer_t**)luaL_checkudata(L, index, "raycaster_renderer");

    if (!handle) {
        luaL_typeerror(L, index, "raycaster_renderer");
    }

    return *handle;
}

static int lua_newrayrenderer(lua_State* L) {
    texture_t* render_texture = luaL_opttexture(L, 2, graphics_get_render_texture());
    raycaster_renderer_t** handle = (raycaster_renderer_t**)lua_newuserdata(L, sizeof(raycaster_renderer_t*));
    *handle = raycaster_renderer_new(render_texture);
    luaL_setmetatable(L, "raycaster_renderer");

    return 1;
}

/**
 * @type Renderer
 */

/**
 * Creates a raycaster renderer object.
 * @function Renderer:new
 * @return @{Renderer}
 */
static int module_raycaster_renderer_new(lua_State* L) {
    return lua_newrayrenderer(L);
}

/**
 * Clears internal buffers.
 * @function Renderer:clear
 * @param target @{string} One of: "all" (default), "color", or "depth". (optional)
 */
static int module_raycaster_renderer_clear(lua_State* L) {
    raycaster_renderer_t* renderer = luaL_checkrayrenderer(L, 1);

    const char* name = luaL_optstring(L, 2, "all");

    if (strcmp(name, "all") == 0) {
        color_t color = (color_t)luaL_optnumber(L, 3, 0);
        float depth = luaL_optnumber(L, 4, FLT_MAX);

        raycaster_renderer_clear_color(renderer, color);
        raycaster_renderer_clear_depth(renderer, depth);
    }
    else if (strcmp(name, "depth") == 0) {
        float depth = luaL_optnumber(L, 3, FLT_MAX);
        raycaster_renderer_clear_depth(renderer, depth);
    }
    else if (strcmp(name, "color") == 0) {
        color_t color = (color_t)luaL_optnumber(L, 3, 0);
        raycaster_renderer_clear_color(renderer, color);
    }
    else {
        luaL_argerror(L, 2, lua_pushfstring(L, "invalid option '%s'", name));
    }

    return 0;
}

#define MAX_PALETTE_SIZE 256
static texture_t* palette[MAX_PALETTE_SIZE];

/**
 * Renders given map.
 * @function Renderer:render
 * @param map @{Map} Map to render.
 * @param tiles @{table} An array of textures. Valid range of indices is 0-255.
*/

/**
 * Renders given sprite.
 * @function Renderer:render
 * @param sprite @{texture} Sprite to render.
 * @param position @{vector2} Position of sprite.
*/
static int module_raycaster_renderer_render(lua_State* L) {
    raycaster_renderer_t* renderer = luaL_checkrayrenderer(L, 1);

    raycaster_map_t** handle = NULL;
    luaL_checktype(L, 2, LUA_TUSERDATA);
    handle = (raycaster_map_t**)luaL_testudata(L, 2, "raycaster_map");

    if (handle) {
        raycaster_map_t* map = *handle;

        if (lua_istable(L, 3)) {
            for (int i = 0; i < MAX_PALETTE_SIZE; i++) {
                lua_pushinteger(L, i);
                lua_gettable(L, 3);

                if (lua_type(L, -1) == LUA_TNIL) {
                    palette[i] = NULL;
                }
                else {
                    palette[i] = luaL_checktexture(L, -1);
                }

                lua_pop(L, 1);
            }

            lua_settop(L, 0);
        }

        raycaster_renderer_render_map(renderer, map, palette);
    }
    else {
        texture_t* sprite = luaL_checktexture(L, 2);
        mfloat_t* position = luaL_checkvector2(L, 3);
        raycaster_renderer_render_sprite(renderer, sprite, position);
    }

    return 0;
}

/**
 * Set renderer's camera data.
 * @function Renderer:camera
 * @param position @{vector2} Camera position.
 * @param direction @{vector2} Camera forward vector.
 * @param fov number Camera field of view in degrees.
*/
static int module_raycaster_renderer_camera(lua_State* L) {
    raycaster_renderer_t* renderer = luaL_checkrayrenderer(L, 1);
    mfloat_t* position = luaL_checkvector2(L, 2);
    mfloat_t* direction = luaL_checkvector2(L, 3);
    float fov = luaL_checknumber(L, 4);

    raycaster_renderer_camera(renderer, position, direction, fov);

    return 0;
}

/**
 * Access renderer's features. If just the feature name is provided, the value of
 * that feature will be returned. If a value is provided, the feature will be set to that value.
 *
 * **Features:**
 *
 *  * <span class="parameter">'fogdistance'</span> number Distance at which fog is fully dark.
 *  * <span class="parameter">'shadetable'</span> @{texture} Texture to use to shade colors using distance. Setting to nil will render full bright.
 *  * <span class="parameter">'drawwalls'</span> boolean Should walls be drawn?
 *  * <span class="parameter">'drawfloors'</span> boolean Should floors be drawn?
 *  * <span class="parameter">'drawceilings'</span> boolean Should ceilings be drawn?
 *
 * @function Renderer:feature
 * @param name @{string} Feature name.
 * @param value Value to set feature to. (optional)
 */
static int module_raycaster_renderer_feature(lua_State* L) {
    raycaster_renderer_t* renderer = luaL_checkrayrenderer(L, 1);
    const char* key = luaL_checkstring(L, 2);
    bool is_setter = lua_gettop(L) > 2;

    if (strcmp(key, "fogdistance") == 0) {
        if (is_setter) {
            float fog_distance = luaL_checknumber(L, 3);
            renderer->features.fog_distance = fog_distance;

            return 0;
        }

        lua_pushnumber(L, renderer->features.fog_distance);

        return 1;
    }
    else if (strcmp(key, "shadetable") == 0) {
        if (is_setter) {
            if (lua_isnil(L, 3)) {
                renderer->features.shade_table = NULL;
            }
            else {
                texture_t* shade_table = luaL_checktexture(L, 3);
                renderer->features.shade_table = shade_table;
            }

            return 0;
        }

        if (renderer->features.shade_table) {
            lua_pushtexture(L, renderer->features.shade_table);
        }
        else {
            lua_pushnil(L);
        }

        return 1;
    }
    else if (strcmp(key, "drawwalls") == 0) {
        if (is_setter) {
            bool draw_walls = lua_toboolean(L, 3);
            renderer->features.draw_walls = draw_walls;

            return 0;
        }

        lua_pushboolean(L, renderer->features.draw_walls);

        return 1;
    }
    else if (strcmp(key, "drawfloors") == 0) {
        if (is_setter) {
            bool draw_floors = lua_toboolean(L, 3);
            renderer->features.draw_floors = draw_floors;

            return 0;
        }

        lua_pushboolean(L, renderer->features.draw_floors);

        return 1;
    }
    else if (strcmp(key, "drawceilings") == 0) {
        if (is_setter) {
            bool draw_ceilings = lua_toboolean(L, 3);
            renderer->features.draw_ceilings = draw_ceilings;

            return 0;
        }

        lua_pushboolean(L, renderer->features.draw_ceilings);

        return 1;
    }
    else {
        luaL_argerror(L, 2, lua_pushfstring(L, "invalid feature '%s'", key));
    }

    return 0;
}

static int module_raycaster_renderer_meta_index(lua_State* L) {
    luaL_checkrayrenderer(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_settop(L, 0);

    luaL_requiref(L, "raycaster", NULL, false);
    lua_getfield(L, -1, "Renderer");
    if (lua_type(L, -1) == LUA_TTABLE) {
        lua_getfield(L, -1, key);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static raycaster_map_t* luaL_checkraycastermap(lua_State* L, int index) {
    raycaster_map_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    handle = (raycaster_map_t**)luaL_checkudata(L, index, "raycaster_map");

    return *handle;
}

static int modules_raycaster_map_meta_index(lua_State* L) {
    raycaster_map_t* map = luaL_checkraycastermap(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_pop(L, -1);

    int* data = NULL;

    if (strcmp(key, "walls") == 0) {
        data = map->walls;
    }
    else if (strcmp(key, "floors") == 0) {
        data = map->floors;
    }
    else if (strcmp(key, "ceilings") == 0) {
        data = map->ceilings;
    }

    if (data) {
        size_t size = map->width * map->height;

        lua_newtable(L);

        for (int i = 0; i < size; i++) {
            lua_pushinteger(L, i + 1);
            lua_pushinteger(L, data[i]);
            lua_settable(L, -3);
        }
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static int modules_raycaster_map_meta_newindex(lua_State* L) {
    raycaster_map_t* map = luaL_checkraycastermap(L, 1);
    const char* key = luaL_checkstring(L, 2);

    int* data = NULL;

    if (strcmp(key, "walls") == 0) {
        data = map->walls;
    }
    else if (strcmp(key, "floors") == 0) {
        data = map->floors;
    }
    else if (strcmp(key, "ceilings") == 0) {
        data = map->ceilings;
    }

    if (data) {
        size_t size = map->width * map->height;
        size_t table_size = lua_rawlen(L, 3);

        if (table_size == size) {
            for (int i = 0; i < size; i++) {
                int index = i + 1;
                lua_pushinteger(L, index);
                lua_gettable(L, 3);

                data[i] = (int)luaL_checknumber(L, -1);

                lua_pop(L, 1);
            }

            lua_settop(L, 0);
        }
        else {
            luaL_error(L, "BAD!");
        }
    }
    else {
        luaL_error(L, "attempt to index a raycaster_map value");
    }

    lua_pop(L, -1);

    return 0;
}

static int modules_raycaster_map_meta_gc(lua_State* L) {
    raycaster_map_t** handle = lua_touserdata(L, 1);
    raycaster_map_free(*handle);
    *handle = NULL;

    return 0;
}

/**
 * @type Map
 */

/**
 * Create a new map.
 * @function Map:new
 * @return @{Map}
*/
static int module_raycaster_map_new(lua_State* L) {
    int width = (int)luaL_checknumber(L, 1);
    int height = (int)luaL_checknumber(L, 2);

    raycaster_map_t** handle = (raycaster_map_t**)lua_newuserdata(L, sizeof(raycaster_map_t*));
    *handle = raycaster_map_new(width, height);
    luaL_setmetatable(L, "raycaster_map");

    return 1;
}

/**
 * Tile indices for walls.
 * @field walls @{texture}
 */

/**
 * Tile indices for floors.
 * @field floors @{texture}
 */

/**
 * Tile indices for ceilings.
 * @field ceilings @{texture}
 */

static const struct luaL_Reg raycaster_map_functions[] = {
    {"new", module_raycaster_map_new},
    {NULL, NULL}
};

static const struct luaL_Reg raycaster_renderer_functions[] = {
    {"new", module_raycaster_renderer_new},
    {"clear", module_raycaster_renderer_clear},
    {"render", module_raycaster_renderer_render},
    {"camera", module_raycaster_renderer_camera},
    {"feature", module_raycaster_renderer_feature},
    {NULL, NULL}
};

static const struct luaL_Reg raycaster_renderer_meta_functions[] = {
    {"__index", module_raycaster_renderer_meta_index},
    {NULL, NULL}
};

static const struct luaL_Reg raycaster_map_meta_functions[] = {
    {"__index", modules_raycaster_map_meta_index},
    {"__newindex", modules_raycaster_map_meta_newindex},
    {"__gc", modules_raycaster_map_meta_gc},
    {NULL, NULL}
};

int luaopen_raycaster(lua_State* L) {
    lua_newtable(L);

    lua_pushstring(L, "Renderer");
    luaL_newlib(L, raycaster_renderer_functions);
    lua_settable(L, -3);

    luaL_newmetatable(L, "raycaster_renderer");
    luaL_setfuncs(L, raycaster_renderer_meta_functions, 0);
    lua_pop(L, 1);

    lua_pushstring(L, "Map");
    luaL_newlib(L, raycaster_map_functions);
    lua_settable(L, -3);

    luaL_newmetatable(L, "raycaster_map");
    luaL_setfuncs(L, raycaster_map_meta_functions, 0);
    lua_pop(L, 1);

    return 1;
}
