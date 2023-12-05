/**
 * Module that extends the standard Lua math library with useful functions for
 * game development.
 *
 * Reference manual for standard math library:
 *
 * <https://www.lua.org/manual/5.4/manual.html#6.7>
 *
 * @module math
 */

#include <stdbool.h>

#include <lua/lua.h>
#include <lua/lauxlib.h>
#include <lua/lualib.h>

#include "../math.h"

/**
 * Clamps given value to specified minimum and maximum range.
 *
 * @function clamp
 * @tparam number value
 * @tparam number min
 * @tparam number max
 * @treturn number Clamped value between min and max.
 */
static int module_math_clamp(lua_State* L) {
    float value = luaL_checknumber(L, 1);
    float min = luaL_checknumber(L, 2);
    float max = luaL_checknumber(L, 3);

    lua_pushnumber(L, clamp(value, min, max));

    return 1;
}

/**
 * Linearly interpolate between a and b by t.
 *
 * @function lerp
 * @tparam number a Start value
 * @tparam number b End value
 * @tparam number t Interpolation value
 * @treturn number Interpolated value between a and b.
 */
static int module_math_lerp(lua_State* L) {
    float a = luaL_checknumber(L, 1);
    float b = luaL_checknumber(L, 2);
    float t = luaL_checknumber(L, 3);

    lua_pushnumber(L, lerp(a, b, t));

    return 1;
}

/**
 * Returns a Perlin noise value.
 *
 * The returned value might be outside the [-1, 1] range. If x, y, and z are
 * all integers the result will be 0.
 *
 * Adapated from reference implementation at: <https://mrl.cs.nyu.edu/~perlin/noise/>
 *
 * @function noise
 * @tparam number x X-coordinate of sample point.
 * @tparam number y Y-coordinate of sample point. (optional)
 * @tparam number z Z-coordinate of sample point. (optional)
 * @treturn number float Sampled point.
 */
static int module_math_noise(lua_State* L) {
    float x = luaL_checknumber(L, 1);
    float y = luaL_optnumber(L, 2, 0);
    float z = luaL_optnumber(L, 3, 0);

    lua_pushnumber(L, noise(x, y, z));

    return 1;
}

/**
 * Interpolates between a and b with smoothing at the limits.
 *
 * @function smoothstep
 * @tparam number a Start value
 * @tparam number b End value
 * @tparam number t Interpolation value
 * @treturn number float
 */
static int module_math_smoothstep(lua_State* L) {
    float a = luaL_checknumber(L, 1);
    float b = luaL_checknumber(L, 2);
    float t = luaL_checknumber(L, 3);

    lua_pushnumber(L, smoothstep(a, b, t));

    return 1;
}

static const struct luaL_Reg module_functions[] = {
    {"clamp", module_math_clamp},
    {"lerp", module_math_lerp},
    {"noise", module_math_noise},
    {"smoothstep", module_math_smoothstep},
    {NULL, NULL}
};

int luaopen_mathextensions(lua_State* L) {
    luaL_requiref(L, "math", NULL, false);

    const luaL_Reg *module;
    for (module = module_functions; module->func; module++) {
        lua_pushstring(L, module->name);
        lua_pushcfunction(L, module->func);
        lua_settable(L, -3);
    }

    lua_pop(L, 1);

    return 1;
}
