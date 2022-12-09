#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "raycaster.h"

static const struct luaL_Reg module_functions[] = {
    {NULL, NULL}
};

int luaopen_raycaster(lua_State* L) {
    luaL_newlib(L, module_functions);
    return 1;
}
