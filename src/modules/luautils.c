#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "luautils.h"

static int error_readonly(lua_State* L) {
    luaL_error(L, "attempt to update a read-only table");
    return 0;
}

int make_readonly(lua_State* L) {
    int base = lua_gettop(L);

    // Given table to make read-only
    luaL_checktype(L, base, LUA_TTABLE); // table

    // Proxy table
    lua_newtable(L);

    // Metatable
    lua_newtable(L);

    // Set __index field of metatable to given table
    lua_pushvalue(L, base);
    lua_setfield(L, -2, "__index");

    // Set __newindex field of metatable to error function
    lua_pushcfunction(L, error_readonly);
    lua_setfield(L, -2, "__newindex");

    // Set proxy table metatable to metatable
    lua_setmetatable(L, -2);

    // Remove given table leaving proxy on top
    lua_remove(L, base);

    return 1;
}

void lua_setdummyfields(lua_State* L, const char* field_names[]) {
    if (!lua_istable(L, -1)) {
        luaL_error(L, "Missing table to set fields for");
        return;
    }

    const char** name;
    for (name = field_names; *name; name++) {
        lua_pushinteger(L, 0);
        lua_setfield(L, -2, *name);
    }
}
