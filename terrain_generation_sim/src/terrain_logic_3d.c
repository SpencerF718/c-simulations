#include "terrain_logic_3d.h"

double perlin_fade(double interpolationFactor) {
    double t = interpolationFactor;
    return t * t * t * (t * (t * 6 - 15) + 10);
}