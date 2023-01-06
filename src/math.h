#ifndef MATH_H
#define MATH_H

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

#endif
