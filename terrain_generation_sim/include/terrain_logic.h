#ifndef TERRAIN_LOGIC_H
#define TERRAIN_LOGIC_H

#include <SDL.h>

// Hard coded permutation table
extern const int permutationTable[];

/*
   Implementation of "smootherstep" function
            6t^5 - 15t^4 + 10t^3 
        =   t^3(6t^2 - 15t + 10)
        =   t * t * t * (t * (t * 6 - 15) + 10)
*/
double perlin_fade(double interpolationFactor);