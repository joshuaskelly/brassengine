#include "math.h"

float clamp(float f, float min, float max) {
    if (f < min) return min;
    if (f > max) return max;
    return f;
}

float frac(float f) {
    return f - (int)f;
}

float max(float a, float b) {
    if (a > b) return a;
    return b;
}

float min(float a, float b) {
    if (a < b) return a;
    return b;
}
