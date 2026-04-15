#ifndef LUAUTILS_H
#define LUAUTILS_H

#include <lua/lua.h>

int make_readonly(lua_State* L);

/**
 * Set dummy fields for autocomplete.
 */
void lua_setdummyfields(lua_State* L, const char* field_names[]);

#endif
