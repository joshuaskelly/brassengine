#include <lua/lua.h>

#include "../event.h"

/* Pushes an event onto the stack as a table.*/
int lua_pushevent(lua_State* L, event_t* event);
