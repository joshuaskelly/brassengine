#ifndef MATH_H
#define MATH_H

#include <stdbool.h>

/**
 * Clamps given value to specified minimum and maximum range.
 *
 * @param f Float value
 * @param min Range minimum
 * @param max Range maximum
 * @return Clamped value for f
 */
float clamp(float f, float min, float max);

/**
 * Returns fractional part of float.
 *
 * @param f Float value
 * @return float Fraction part of value between 0 and 1
 */
float frac(float f);

/**
 * Linearly interpolate between a and b by t.
 *
 * @param a Start value
 * @param b End value
 * @param t Interpolation value between a and b.
 * @return Interpolated value between a and b.
 */
float lerp(float a, float b, float t);

/**
 * Returns a Perlin noise value.
 *
 * The returned value might be outside the [-1, 1] range. If x, y, and z are
 * all integers the result will be 0.
 *
 * Adapated from: https://mrl.cs.nyu.edu/~perlin/noise/
 *
 * @param x
 * @param y
 * @param z
 * @return float
 */
float noise(float x, float y, float z);

/**
 * Interpolates between a and b with smoothing at the limits.
 *
 * @param a Start value
 * @param b End value
 * @param t Interpolation value
 * @return float
 */
float smoothstep(float a, float b, float t);

/**
 * Return sign of given number. 1 if greater than zero, -1 if less than zero, and
 * 0 if equal to zero.
 *
 * @param f
 * @return float
 */
float sign(float f);

/**
 * Returns modulo of a by b.
 *
 * @param a
 * @param b
 * @return int
 */
int modulo(int a, int b);

/**
 * Returns modulo of a by b.
 *
 * @param a
 * @param b
 * @return float
 */
float modulof(float a, float b);

/**
 * Returns if a and b are similar in value.
 *
 * @param a
 * @param b
 * @return bool true if approximately equal, false otherwise
 */
bool approximately(float a, float b);

#endif
