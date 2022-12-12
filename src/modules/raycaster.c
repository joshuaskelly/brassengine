#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include <mathc/mathc.h>

#include "raycaster.h"
#include "texture.h"

#include "../renderers/raycaster.h"

static int module_raycast_render(lua_State* L) {
    float px = luaL_checknumber(L, 1);
    float py = luaL_checknumber(L, 2);
    float dx = luaL_checknumber(L, 3);
    float dy = luaL_checknumber(L, 4);
    float fov = luaL_checknumber(L, 5);
    texture_t** texture = luaL_checktexture(L, 6);

    mfloat_t position[VEC2_SIZE] = {px, py};
    mfloat_t direction[VEC2_SIZE] = {dx, dy};

    raycaster_render(&position, &direction, fov, *texture);
    return 0;
}

static const struct luaL_Reg module_functions[] = {
    {"render", module_raycast_render},
    {NULL, NULL}
};

int luaopen_raycaster(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
