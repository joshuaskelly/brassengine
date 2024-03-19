#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "event.h"
#include "../event.h"

static void push_fields(lua_State* L, event_t* event) {
    switch (event->type) {
        case EVENT_QUIT:
            lua_pushstring(L, "type");
            lua_pushstring(L, "quit");
            lua_settable(L, -3);

            break;
        case EVENT_RELOAD:
            lua_pushstring(L, "type");
            lua_pushstring(L, "reload");

            lua_settable(L, -3);
            break;
        case EVENT_KEYDOWN:
            lua_pushstring(L, "type");
            lua_pushstring(L, "keydown");
            lua_settable(L, -3);

            lua_pushstring(L, "code");
            lua_pushinteger(L, event->key.code);
            lua_settable(L, -3);

            lua_pushstring(L, "symbol");
            lua_pushinteger(L, event->key.symbol);
            lua_settable(L, -3);

            break;
        case EVENT_KEYUP:
            lua_pushstring(L, "type");
            lua_pushstring(L, "keyup");
            lua_settable(L, -3);

            lua_pushstring(L, "code");
            lua_pushinteger(L, event->key.code);
            lua_settable(L, -3);

            lua_pushstring(L, "symbol");
            char symbol[2] = {event->key.symbol, '\0'};
            lua_pushstring(L, symbol);
            lua_settable(L, -3);

            break;
        case EVENT_MOUSEMOTION:
            lua_pushstring(L, "type");
            lua_pushstring(L, "mousemotion");
            lua_settable(L, -3);

            lua_pushstring(L, "x");
            lua_pushinteger(L, event->motion.x);
            lua_settable(L, -3);

            lua_pushstring(L, "y");
            lua_pushinteger(L, event->motion.y);
            lua_settable(L, -3);

            lua_pushstring(L, "motionx");
            lua_pushinteger(L, event->motion.motion_x);
            lua_settable(L, -3);

            lua_pushstring(L, "motiony");
            lua_pushinteger(L, event->motion.motion_y);
            lua_settable(L, -3);

            break;
        case EVENT_MOUSEDOWN:
            lua_pushstring(L, "type");
            lua_pushstring(L, "mousedown");
            lua_settable(L, -3);

            lua_pushstring(L, "button");
            lua_pushinteger(L, event->button.button);
            lua_settable(L, -3);

            break;
        case EVENT_MOUSEUP:
            lua_pushstring(L, "type");
            lua_pushstring(L, "mouseup");
            lua_settable(L, -3);

            lua_pushstring(L, "button");
            lua_pushinteger(L, event->button.button);
            lua_settable(L, -3);

            break;
        default:
            lua_pushstring(L, "type");
            lua_pushstring(L, "unset");
            lua_settable(L, -3);

    }
}

int lua_pushevent(lua_State* L, event_t* event) {
    lua_newtable(L);
    push_fields(L, event);

    return 1;
}
