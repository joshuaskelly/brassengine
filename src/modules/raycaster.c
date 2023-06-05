/**
 * Module for raycaster renderer.
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

raycaster_camera_t* luaL_checkraycastercamera(lua_State* L, int index) {
    raycaster_camera_t* camera = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    camera = (raycaster_camera_t*)luaL_checkudata(L, index, "raycaster_camera");

    if (!camera) {
        luaL_typeerror(L, index, "raycaster_camera");
    }

    return camera;
}

static int modules_raycaster_camera_meta_index(lua_State* L) {
    raycaster_camera_t* camera = luaL_checkraycastercamera(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_pop(L, -1);

    if (strcmp(key, "position") == 0) {
        lua_newvector2(L, camera->position[0], camera->position[1]);
    }
    else if (strcmp(key, "direction") == 0) {
        lua_newvector2(L, camera->direction[0], camera->direction[1]);
    }
    else if (strcmp(key, "fov") == 0) {
        lua_pushnumber(L, camera->fov);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static int modules_raycaster_camera_meta_newindex(lua_State* L) {
    raycaster_camera_t* camera = luaL_checkraycastercamera(L, 1);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "position") == 0) {
        mfloat_t* vector = luaL_checkvector2(L, 3);
        vec2_assign(camera->position, vector);
    }
    else if (strcmp(key, "direction") == 0) {
        mfloat_t* vector = luaL_checkvector2(L, 3);
        vec2_assign(camera->direction, vector);
    }
    else if (strcmp(key, "fov") == 0) {
        float number = luaL_checknumber(L, 3);
        camera->fov = number;
    }
    else {
        luaL_error(L, "attempt to index a raycaster_camera value");
    }

    lua_pop(L, -1);

    return 0;
}

static const struct luaL_Reg raycaster_camera_meta_functions[] = {
    {"__index", modules_raycaster_camera_meta_index},
    {"__newindex", modules_raycaster_camera_meta_newindex},
    {NULL, NULL}
};

raycaster_sprite_t* luaL_checkraycastersprite(lua_State* L, int index) {
    raycaster_sprite_t* sprite = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    sprite = (raycaster_sprite_t*)luaL_checkudata(L, index, "raycaster_sprite");

    if (!sprite) {
        luaL_typeerror(L, index, "raycaster_sprite");
    }

    return sprite;
}

static int lua_newraycastersprite(lua_State* L) {
    raycaster_sprite_t* sprite = (raycaster_sprite_t*)lua_newuserdata(L, sizeof(raycaster_sprite_t));
    sprite->texture = NULL;
    sprite->position[0] = 0;
    sprite->position[1] = 0;
    sprite->distance = FLT_MAX;
    luaL_setmetatable(L, "raycaster_sprite");

    return 1;
}

static int modules_raycaster_sprite_meta_index(lua_State* L) {
    raycaster_sprite_t* sprite = luaL_checkraycastersprite(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_pop(L, -1);

    if (strcmp(key, "position") == 0) {
        lua_newvector2(L, sprite->position[0], sprite->position[1]);
    }
    else if (strcmp(key, "texture") == 0) {
        lua_pushtexture(L, sprite->texture);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static int modules_raycaster_sprite_meta_newindex(lua_State* L) {
    raycaster_sprite_t* sprite = luaL_checkraycastersprite(L, 1);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "position") == 0) {
        mfloat_t* vector = luaL_checkvector2(L, 3);
        vec2_assign(sprite->position, vector);
    }
    else if (strcmp(key, "texture") == 0) {
        texture_t* texture = luaL_checktexture(L, 3);
        sprite->texture = texture;
    }
    else {
        luaL_error(L, "attempt to index a raycaster_sprite value");
    }

    lua_pop(L, -1);

    return 0;
}

static const struct luaL_Reg raycaster_sprite_meta_functions[] = {
    {"__index", modules_raycaster_sprite_meta_index},
    {"__newindex", modules_raycaster_sprite_meta_newindex},
    {NULL, NULL}
};

raycaster_map_t* luaL_checkraycastermap(lua_State* L, int index) {
    raycaster_map_t** handle = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    handle = (raycaster_map_t**)luaL_checkudata(L, index, "raycaster_map");

    return *handle;
}

static int modules_raycaster_map_add_sprite(lua_State* L) {
    raycaster_map_t* map = luaL_checkraycastermap(L, 1);
    raycaster_sprite_t* sprite = luaL_checkraycastersprite(L, 2);

    lua_pop(L, -1);

    raycaster_map_add_sprite(map, sprite);

    return 0;
}

static int modules_raycaster_map_remove_sprite(lua_State* L) {
    raycaster_map_t* map = luaL_checkraycastermap(L, 1);
    raycaster_sprite_t* sprite = luaL_checkraycastersprite(L, 2);

    lua_pop(L, -1);

    raycaster_map_remove_sprite(map, sprite);

    return 0;
}

static int modules_raycaster_map_meta_index(lua_State* L) {
    raycaster_map_t* map = luaL_checkraycastermap(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_pop(L, -1);

    if (strcmp(key, "walls") == 0) {
        lua_pushtexture(L, map->walls);
    }
    else if (strcmp(key, "add_sprite") == 0) {
        lua_pushcfunction(L, modules_raycaster_map_add_sprite);
    }
    else if (strcmp(key, "remove_sprite") == 0) {
        lua_pushcfunction(L, modules_raycaster_map_remove_sprite);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static int modules_raycaster_map_meta_newindex(lua_State* L) {
    raycaster_map_t* map = luaL_checkraycastermap(L, 1);
    const char* key = luaL_checkstring(L, 2);

    if (strcmp(key, "walls") == 0) {
        texture_t* texture = luaL_checktexture(L, 3);
        map->walls = texture;
    }
    else {
        luaL_error(L, "attempt to index a raycaster_mapmap value");
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

static const struct luaL_Reg raycaster_map_meta_functions[] = {
    {"__index", modules_raycaster_map_meta_index},
    {"__newindex", modules_raycaster_map_meta_newindex},
    {"__gc", modules_raycaster_map_meta_gc},
    {NULL, NULL}
};

/**
 * Create a new raycaster_map object.
 * @function map_new
 * @return raycaster_map
 */
static int modules_raycaster_map_new(lua_State* L) {
    raycaster_map_t** handle = (raycaster_map_t**)lua_newuserdata(L, sizeof(raycaster_map_t*));
    *handle = raycaster_map_new();
    luaL_setmetatable(L, "raycaster_map");

    return 1;
}

/**
 * Create a new raycaster_camera object.
 * @function camera_new
 * @return raycaster_camera
 */
static int modules_raycaster_camera_new(lua_State* L) {
    raycaster_camera_t* camera = (raycaster_camera_t*)lua_newuserdata(L, sizeof(raycaster_camera_t));
    camera->position[0] = 0;
    camera->position[1] = 0;
    camera->direction[0] = 1;
    camera->direction[1] = 0;
    camera->fov = 90;
    luaL_setmetatable(L, "raycaster_camera");

    return 1;
}

/**
 * Create a new raycaster_sprite object.
 * @function sprite_new
 * @return raycaster_sprite
 */
static int modules_raycaster_sprite_new(lua_State* L) {
    lua_newraycastersprite(L);

    return 1;
}

/**
 * Renders given map data.
 * @function render
 * @param camera Camera to render with
 * @param map Map as texture
 * @param texture Render texture
 */

/**
 * Renders given map data.
 * @function render
 * @param camera Camera to render with
 * @param map Map as texture
 * @param texture Render texture
 * @param rx Render texture x-offset
 * @param ry Render texture y-offset
 * @param rw Render texture width
 * @param rh Render texture height
 */
static int module_raycaster_render(lua_State* L) {
    raycaster_camera_t* camera = luaL_checkraycastercamera(L, 1);
    raycaster_map_t* map = luaL_checkraycastermap(L, 2);
    texture_t* render_texture = luaL_checktexture(L, 3);
    float rx = luaL_optnumber(L, 4, 0);
    float ry = luaL_optnumber(L, 5, 0);
    float rw = luaL_optnumber(L, 6, render_texture->width);
    float rh = luaL_optnumber(L, 7, render_texture->height);

    lua_pop(L, 1);

    rect_t rect = {rx, ry, rw, rh};

    raycaster_render(
        camera,
        map,
        render_texture,
        &rect
    );

    return 0;
}

/**
 * Sets texture in texture palette.
 * @function palette
 * @param index Texture palette index
 * @param texture Texture for given index
 */
static int module_raycaster_palette(lua_State* L) {
    int i = (int)luaL_checknumber(L, 1);
    texture_t* texture = luaL_checktexture(L, 2);

    lua_pop(L, 1);

    raycaster_set_texture(i, texture);

    return 0;
}

/**
 * Sets texture to use as a color lookup table for shading.
 * @function shade_table
 * @param texture Texture to use as lookup table.
 */
static int module_raycaster_shade_table_set(lua_State* L) {
    texture_t* texture = luaL_checktexture(L, 1);

    lua_pop(L, 1);

    raycaster_shade_table_set(texture);

    return 0;
}

/**
 * Sets far distance for fog.
 * @function fog_distance
 * @param distance Distance where fog effect is greatest.
 */
static int module_raycaster_fog_distance_set(lua_State* L) {
    float distance = luaL_checknumber(L, 1);

    raycaster_fog_distance_set(distance);

    return 0;
}

/**
 * Map for rendering a raycaster scene.
 * @type raycaster_map
 */

/**
 * Map walls.
 * @field walls A texture userdata representing wall positions and textures.
 */

/**
 * Add given sprite to map.
 * @function add_sprite
 * @param sprite A raycaster_sprite
 */

/**
 * Remove given sprite from map.
 * @function remove_sprite
 * @param sprite A raycaster_sprite
 */

/**
 * Camera for rendering a raycaster scene.
 * @type raycaster_camera
 */

/**
 * Camera position.
 * @field position A vector2 userdata representing camera position.
 */

/**
 * Camera direction.
 * @field direction A vector2 userdata representing camera direction.
 */

/**
 * Camera field of view.
 * @field fov Camera field of view in degrees.
 */

/**
 * Sprite for rendering in a raycaster scene.
 * @type raycaster_sprite
 */

/**
 * Sprite position.
 * @field position A vector2 userdata representing sprite position.
 */

/**
 * Sprite texture.
 * @field texture
 */

static const struct luaL_Reg module_functions[] = {
    {"render", module_raycaster_render},
    {"palette", module_raycaster_palette},
    {"shade_table", module_raycaster_shade_table_set},
    {"fog_distance", module_raycaster_fog_distance_set},
    {"map_new", modules_raycaster_map_new},
    {"camera_new", modules_raycaster_camera_new},
    {"sprite_new", modules_raycaster_sprite_new},
    {NULL, NULL}
};

int luaopen_raycaster(lua_State* L) {
    luaL_newlib(L, module_functions);

    luaL_newmetatable(L, "raycaster_map");
    luaL_setfuncs(L, raycaster_map_meta_functions, 0);
    lua_pop(L, 1);

    luaL_newmetatable(L, "raycaster_camera");
    luaL_setfuncs(L, raycaster_camera_meta_functions, 0);
    lua_pop(L, 1);

    luaL_newmetatable(L, "raycaster_sprite");
    luaL_setfuncs(L, raycaster_sprite_meta_functions, 0);
    lua_pop(L, 1);

    return 1;
}
