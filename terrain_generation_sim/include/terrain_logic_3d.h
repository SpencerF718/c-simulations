#ifndef TERRAIN_LOGIC_3D_H
#define TERRAIN_LOGIC_3D_H

#include <stdint.h>
#include <math.h>

typedef struct {
    int width;
    int height;
    int depth;
    float* data;
} Terrain3D;

/*
   Implementation of "smootherstep" function
            6t^5 - 15t^4 + 10t^3
        =   t^3(6t^2 - 15t + 10)
        =   t * t * t * (t * (t * 6 - 15) + 10)
*/
double perlin_fade(double interpolationFactor);

#endif
