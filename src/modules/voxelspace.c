#include <string.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "voxelspace.h"
#include "vector2.h"

#include "../renderers/voxelspace.h"

voxelspace_camera_t* luaL_checkvoxelspacecamera(lua_State* L, int index) {
    voxelspace_camera_t* camera = NULL;
    luaL_checktype(L, index, LUA_TUSERDATA);
    camera = (voxelspace_camera_t*)luaL_checkudata(L, index, "voxelspace_camera");

    if (!camera) {
        luaL_typeerror(L, index, "voxelspace_camera");
    }

    return camera;
}

static int module_voxelspace_camera_new(lua_State* L) {
    voxelspace_camera_t* camera = (voxelspace_camera_t*)lua_newuserdata(L, sizeof(voxelspace_camera_t));
    camera->position[0] = 0;
    camera->position[1] = 0;
    camera->direction[0] = 1;
    camera->direction[1] = 0;
    camera->fov = 90;
    camera->far = 100;
    luaL_setmetatable(L, "voxelspace_camera");

    return 1;
}


static int module_voxelspace_camera_meta_index(lua_State* L) {
    voxelspace_camera_t* camera = luaL_checkvoxelspacecamera(L, 1);
    const char* key = luaL_checkstring(L, 2);

    lua_pop(L, -1);

    if (strncmp(key, "position", 8) == 0) {
        lua_newvector2(L, camera->position[0], camera->position[1]);
    }
    else if (strncmp(key, "direction", 9) == 0) {
        lua_newvector2(L, camera->direction[0], camera->direction[1]);
    }
    else if (strncmp(key, "fov", 3) == 0) {
        lua_pushnumber(L, camera->fov);
    }
    else if (strncmp(key, "far", 3) == 0) {
        lua_pushnumber(L, camera->far);
    }
    else {
        lua_pushnil(L);
    }

    return 1;
}

static int module_voxelspace_camera_meta_newindex(lua_State* L) {
    voxelspace_camera_t* camera = luaL_checkvoxelspacecamera(L, 1);
    const char* key = luaL_checkstring(L, 2);

    if (strncmp(key, "position", 8) == 0) {
        mfloat_t* vector = luaL_checkvector2(L, 3);
        vec2_assign(camera->position, vector);
    }
    else if (strncmp(key, "direction", 9) == 0) {
        mfloat_t* vector = luaL_checkvector2(L, 3);
        vec2_assign(camera->direction, vector);
    }
    else if (strncmp(key, "fov", 3) == 0) {
        float number = luaL_checknumber(L, 3);
        camera->fov = number;
    }
    else if (strncmp(key, "far", 3) == 0) {
        float number = luaL_checknumber(L, 3);
        camera->far = number;
    }
    else {
        luaL_error(L, "attempt to index a voxelspace_camera value");
    }

    lua_pop(L, -1);

    return 0;
}

static const struct luaL_Reg voxelspace_camera_meta_functions[] = {
    {"__index", module_voxelspace_camera_meta_index},
    {"__newindex", module_voxelspace_camera_meta_newindex},
    {NULL, NULL}
};

static const struct luaL_Reg module_functions[] = {
    //{"render", module_voxelspace_render},
    //{"palette", module_voxelspace_palette},
    //{"shade_table", module_voxelspace_shade_table_set},
    //{"fog_distance", module_voxelspace_fog_distance_set},
    //{"map_new", module_voxelspace_map_new},
    {"camera_new", module_voxelspace_camera_new},
    //{"sprite_new", modules_raycaster_sprite_new},
    {NULL, NULL}
};

int luaopen_voxelspace(lua_State* L) {
    luaL_newlib(L, module_functions);

    //luaL_newmetatable(L, "raycaster_map");
    //luaL_setfuncs(L, raycaster_map_meta_functions, 0);
    //lua_pop(L, 1);

    luaL_newmetatable(L, "voxelspace_camera");
    luaL_setfuncs(L, voxelspace_camera_meta_functions, 0);
    lua_pop(L, 1);

    //luaL_newmetatable(L, "raycaster_sprite");
    //luaL_setfuncs(L, raycaster_sprite_meta_functions, 0);
    //lua_pop(L, 1);

    return 1;
}