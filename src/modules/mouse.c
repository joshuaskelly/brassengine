/**
 * Module for getting mouse input.
 * @module mouse
 */
#include <stdbool.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "mouse.h"

#include "../input.h"
#include "../platform.h"

/**
 * Check if given button is down.
 * @function button
 * @tparam integer button Button index to check. Primary button is 1, middle button is 2,
 * secondary button is 3.
 * @treturn boolean True if button is down, false otherwise
 */
static int modules_mouse_button_get(lua_State* L) {
    int button = luaL_checkinteger(L, 1);

    lua_pop(L, -1);

    bool is_pressed = input_mouse_is_button_pressed(button);
    lua_pushboolean(L, is_pressed);

    return 1;
}

/**
 * Get current mouse position.
 * @function position
 * @treturn[1] integer Mouse x coord
 * @treturn[2] integer Mouse y coord
 */
static int modules_mouse_position_get(lua_State* L) {
    int x = 0;
    int y = 0;

    input_mouse_position(&x, &y);

    lua_pushinteger(L, x);
    lua_pushinteger(L, y);

    return 2;
}

/**
 * Get mouse relative motion.
 * @function motion
 * @treturn[1] integer Mouse x coord
 * @treturn[2] integer Mouse y coord
 */
static int modules_mouse_motion_get(lua_State* L) {
    int rel_x;
    int rel_y;

    input_mouse_motion(&rel_x, &rel_y);

    lua_pushnumber(L, rel_x);
    lua_pushnumber(L, rel_y);

    return 2;
}

/**
 * Get mouse wheel scroll values.
 * @function wheel
 * @treturn[1] integer Horizontal scroll amount
 * @treturn[2] integer Vertical scroll amount
 */
static int modules_mouse_wheel_get(lua_State* L) {
    int horizontal;
    int vertical;

    input_mouse_wheel(&horizontal, &vertical);

    lua_pushnumber(L, horizontal);
    lua_pushnumber(L, vertical);

    return 2;
}

/**
 * Set mouse grab state.
 * @function set_grabbed
 * @tparam boolean grabbed True if mouse should be constrained to window.
 */
static int modules_mouse_grabbed_set(lua_State* L) {
    bool grabbed = lua_toboolean(L, 1);

    lua_settop(L, 0);

    platform_mouse_grabbed_set(grabbed);

    return 0;
}

/**
 * Get mouse grab state.
 *
 * @treturn boolean True if mouse is grabbed, false otherwise.
 */
static int modules_mouse_grabbed_get(lua_State* L) {
    bool grabbed = platform_mouse_grabbed_get();

    lua_pushboolean(L, grabbed);

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
 * @tfield integer LEFT
 * @tfield integer MIDDLE
 * @tfield integer RIGHT
 */

static const struct luaL_Field mouse_buttons[] = {
    {"LEFT", 1},
    {"MIDDLE", 2},
    {"RIGHT", 3},
    {NULL, 0}
};

static int mouse_button_enums(lua_State*L) {
    lua_newtable(L);

    const struct luaL_Field* button;
    for (button = mouse_buttons; button->name; button++) {
        lua_pushinteger(L, button->value);
        lua_setfield(L, -2, button->name);
    }

    return 1;
}

static const struct luaL_Reg modules_mouse_functions[] = {
    {"button", modules_mouse_button_get},
    {"position", modules_mouse_position_get},
    {"motion", modules_mouse_motion_get},
    {"wheel", modules_mouse_wheel_get},
    {"set_grabbed", modules_mouse_grabbed_set},
    {"get_grabbed", modules_mouse_grabbed_get},
    {NULL, NULL}
};

int luaopen_mouse(lua_State* L) {
    luaL_newlib(L, modules_mouse_functions);

    mouse_button_enums(L);
    lua_setfield(L, -2, "buttons");

    return 1;
}
