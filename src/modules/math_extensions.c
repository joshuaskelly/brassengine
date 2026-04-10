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
static int modules_math_clamp(lua_State* L) {
    float value = luaL_checknumber(L, 1);
    float min = luaL_checknumber(L, 2);
    float max = luaL_checknumber(L, 3);

    lua_pushnumber(L, clamp(value, min, max));

    return 1;
}

/**
 * Returns fractional part of float.
 *
 * @function frac
 * @tparam number value
 * @treturn number Fractional part of float.
 */
static int modules_math_frac(lua_State* L) {
    float f = luaL_checknumber(L, 1);

    lua_pushnumber(L, frac(f));

    return 1;
}

/**
 * Returns multiple of t nearest to value.
 *
 * @function snap
 * @tparam number value
 * @tparam number t Value to snap to
 * @treturn number Multiple of t nearest to given value
 */
static int modules_math_snap(lua_State* L) {
    float f = luaL_checknumber(L, 1);
    float t = luaL_checknumber(L, 2);

    lua_pushnumber(L, snap(f, t));

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
static int modules_math_lerp(lua_State* L) {
    float a = luaL_checknumber(L, 1);
    float b = luaL_checknumber(L, 2);
    float t = luaL_checknumber(L, 3);

    lua_pushnumber(L, lerp(a, b, t));

    return 1;
}

/**
 * Linearly interpolate between a and b by t.
 *
 * @function inverse_lerp
 * @tparam number a Start value
 * @tparam number b End value
 * @tparam number v Value in range [a, b]
 * @treturn number Value where v is between a and b in range [0, 1].
 */
static int modules_math_inverse_lerp(lua_State* L) {
    float a = luaL_checknumber(L, 1);
    float b = luaL_checknumber(L, 2);
    float v = luaL_checknumber(L, 3);

    lua_pushnumber(L, inverse_lerp(a, b, v));

    return 1;
}

/**
 * Linearly map given value from source range [a, b] to corresponding value in
 * dest range [c, d].
 *
 * @function remap
 * @tparam number a Source range start value
 * @tparam number b Source range end value
 * @tparam number c Destination range start value
 * @tparam number d Destination range end value
 * @tparam number v Value in start range
 * @treturn number Corresponding value in range [c, d]
 */
static int modules_math_remap(lua_State* L) {
    float a = luaL_checknumber(L, 1);
    float b = luaL_checknumber(L, 2);
    float c = luaL_checknumber(L, 3);
    float d = luaL_checknumber(L, 4);
    float v = luaL_checknumber(L, 5);

    lua_pushnumber(L, remap(a, b, c, d, v));

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
static int modules_math_noise(lua_State* L) {
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
static int modules_math_smoothstep(lua_State* L) {
    float a = luaL_checknumber(L, 1);
    float b = luaL_checknumber(L, 2);
    float t = luaL_checknumber(L, 3);

    lua_pushnumber(L, smoothstep(a, b, t));

    return 1;
}

/**
 * Returns if a and b are similar in value.
 *
 * @function approximately
 * @tparam number a
 * @tparam number b
 * @treturn boolean
 */
static int modules_math_approximately(lua_State* L) {
    float a = luaL_checknumber(L, 1);
    float b = luaL_checknumber(L, 2);

    lua_pushboolean(L, approximately(a, b));

    return 1;
}

/**
 * Return sign of given number. 1 if greater than zero, -1 if less than zero, and
 * 0 if equal to zero.
 *
 * @function sign
 * @tparam number f
 * @return number float
 */
static int modules_math_sign(lua_State* L) {
    float a = luaL_checknumber(L, 1);

    lua_pushnumber(L, sign(a));

    return 1;
}

static const struct luaL_Reg modules_math_functions[] = {
    {"clamp", modules_math_clamp},
    {"frac", modules_math_frac},
    {"snap", modules_math_snap},
    {"lerp", modules_math_lerp},
    {"inverselerp", modules_math_inverse_lerp},
    {"remap", modules_math_remap},
    {"noise", modules_math_noise},
    {"smoothstep", modules_math_smoothstep},
    {"sign", modules_math_sign},
    {"approximately", modules_math_approximately},
    {NULL, NULL}
};

int luaopen_mathextensions(lua_State* L) {
    luaL_requiref(L, "math", NULL, false);

    const luaL_Reg *module;
    for (module = modules_math_functions; module->func; module++) {
        lua_pushstring(L, module->name);
        lua_pushcfunction(L, module->func);
        lua_settable(L, -3);
    }

    lua_pop(L, 1);

    return 1;
}
