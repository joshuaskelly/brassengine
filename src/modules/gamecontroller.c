/**
 * Module for getting gamecontroller input.
 * @module gamecontroller
 */
#include <ctype.h>
#include <stdbool.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "gamecontroller.h"

#include "../input.h"

/**
 * Functions
 * @section Functions
 */

/**
 * Check if given button is down.
 * @function button
 * @tparam integer button Button to check.
 * @treturn boolean True if button is down.
 */
static int modules_gamecontroller_button_get(lua_State* L) {
    int id = luaL_checknumber(L, 1);
    int button = luaL_checknumber(L, 2);
    lua_settop(L, 0);

    bool is_down = input_controller_is_button_pressed(id, button);
    lua_pushboolean(L, is_down);

    return 1;
}

/**
 * Get current value of given axis.
 * @function axis
 * @tparam integer axis Axis to check.
 * @return number
 */
static int modules_gamecontroller_axis_get(lua_State* L) {
    int id = luaL_checknumber(L, 1);
    int axis = luaL_checknumber(L, 2);
    lua_settop(L, 0);

    float value = 0;
    input_controller_axis(id, axis, &value);
    lua_pushnumber(L, value);

    return 1;
}

/**
 * Get number of connected controllers.
 * @function count
 * @return int
 */
static int modules_gamecontroller_count_get(lua_State* L) {
    int count = input_controller_count();
    lua_pushinteger(L, count);

    return 1;
}

/**
 * Get an array of connected controller ids.
 * @function connected
 * @return {integer, ...}
 */

/**
 * Check if given controller id is connected.
 * @function connected
 * @tparam integer id Controller id to check.
 * @return true if given id is connected, false otherwise.
 */
static int modules_gamecontroller_connected_get(lua_State* L) {
    if (lua_gettop(L) == 0) {
        uint8_t ids[input_controller_count()];
        input_controller_ids(ids);

        lua_newtable(L);

        for (int i = 0; i < input_controller_count(); i++) {
            lua_pushinteger(L, i + 1);
            lua_pushinteger(L, ids[i]);
            lua_settable(L, -3);
        }
    }
    else {
        int id = luaL_checknumber(L, 1);
        lua_pushboolean(L, input_controller_connected(id));
    }

    return 1;
}

struct luaL_Field {
  const char *name;
  uint8_t value;
};


/**
 * Enums
 * @section Enums
 */

/**
 * Button enums
 * @table buttons
 *
 * @tfield integer A
 * @tfield integer B
 * @tfield integer X
 * @tfield integer Y
 * @tfield integer BACK
 * @tfield integer GUIDE
 * @tfield integer START
 * @tfield integer LEFTSTICK
 * @tfield integer RIGHTSTICK
 * @tfield integer LEFTSHOULDER
 * @tfield integer RIGHTSHOULDER
 * @tfield integer DPAD_UP
 * @tfield integer DPAD_DOWN
 * @tfield integer DPAD_LEFT
 * @tfield integer DPAD_RIGHT
 */

static const struct luaL_Field gamecontroller_buttons[] = {
    {"A", CONTROLLER_BUTTON_A},
    {"B", CONTROLLER_BUTTON_B},
    {"X", CONTROLLER_BUTTON_X},
    {"Y", CONTROLLER_BUTTON_Y},
    {"BACK", CONTROLLER_BUTTON_BACK},
    {"GUIDE", CONTROLLER_BUTTON_GUIDE},
    {"START", CONTROLLER_BUTTON_START},
    {"LEFTSTICK", CONTROLLER_BUTTON_LEFTSTICK},
    {"RIGHTSTICK", CONTROLLER_BUTTON_RIGHTSTICK},
    {"LEFTSHOULDER", CONTROLLER_BUTTON_LEFTSHOULDER},
    {"RIGHTSHOULDER", CONTROLLER_BUTTON_RIGHTSHOULDER},
    {"DPAD_UP", CONTROLLER_BUTTON_DPAD_UP},
    {"DPAD_DOWN", CONTROLLER_BUTTON_DPAD_DOWN},
    {"DPAD_LEFT", CONTROLLER_BUTTON_DPAD_LEFT},
    {"DPAD_RIGHT", CONTROLLER_BUTTON_DPAD_RIGHT},
    {NULL, 0}
};

static int gamecontroller_button_enums(lua_State*L) {
    lua_newtable(L);

    const struct luaL_Field* button;
    for (button = gamecontroller_buttons; button->name; button++) {
        lua_pushinteger(L, button->value);
        lua_setfield(L, -2, button->name);
    }

    return 1;
}

/**
 * Axes enums
 * @table axes
 *
 * @tfield integer LEFTX
 * @tfield integer LEFTY
 * @tfield integer RIGHTX
 * @tfield integer RIGHTY
 * @tfield integer TRIGGERLEFT
 * @tfield integer TRIGGERRIGHT
 */

static const struct luaL_Field gamecontroller_axes[] = {
    {"LEFTX", CONTROLLER_AXIS_LEFTX},
    {"LEFTY", CONTROLLER_AXIS_LEFTY},
    {"RIGHTX", CONTROLLER_AXIS_RIGHTX},
    {"RIGHTY", CONTROLLER_AXIS_RIGHTY},
    {"TRIGGERLEFT", CONTROLLER_AXIS_TRIGGERLEFT},
    {"TRIGGERRIGHT", CONTROLLER_AXIS_TRIGGERRIGHT},
    {NULL, 0}
};

static int gamecontroller_button_axes(lua_State*L) {
    lua_newtable(L);

    const struct luaL_Field* axis;
    for (axis = gamecontroller_axes; axis->name; axis++) {
        lua_pushinteger(L, axis->value);
        lua_setfield(L, -2, axis->name);
    }

    return 1;
}

static int error_readonly(lua_State* L) {
    luaL_error(L, "attempt to update a read-only table");
    return 0;
}

static int make_readonly(lua_State* L) {
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

static const struct luaL_Reg modules_gamecontroller_functions[] = {
    {"button", modules_gamecontroller_button_get},
    {"axis", modules_gamecontroller_axis_get},
    {"count", modules_gamecontroller_count_get},
    {"connected", modules_gamecontroller_connected_get},
    {NULL, NULL}
};

int luaopen_gamecontroller(lua_State* L) {
    luaL_newlib(L, modules_gamecontroller_functions);

    gamecontroller_button_enums(L);
    //make_readonly(L);
    lua_setfield(L, -2, "buttons");

    gamecontroller_button_axes(L);
    //make_readonly(L);
    lua_setfield(L, -2, "axes");

    return 1;
}
