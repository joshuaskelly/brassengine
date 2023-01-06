#include "math.h"

float clamp(float f, float min, float max) {
    if (f < min) return min;
    if (f > max) return max;
    return f;
}

float frac(float f) {
    return f - (int)f;
}
