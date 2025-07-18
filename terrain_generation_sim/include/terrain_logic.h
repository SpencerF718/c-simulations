#ifndef TERRAIN_LOGIC_H
#define TERRAIN_LOGIC_H

#include <SDL.h>
#include <math.h>
#include <stdint.h>

// Hard coded permutation table
extern const uint8_t permutationTable[];

// 2D array which holds gradient vectors
extern const double gradientVectors[16][2];

/*
   Implementation of "smootherstep" function
            6t^5 - 15t^4 + 10t^3 
        =   t^3(6t^2 - 15t + 10)
        =   t * t * t * (t * (t * 6 - 15) + 10)
*/
double perlin_fade(double interpolationFactor);

// Calculates dot product of a selected gradient vector and distance vector.
double gradient_dot_product(uint8_t permutationHash, double distanceX, double distanceY);

// Generates a 2D Perlin noise value for x and y
double perlin_noise_2d(double xCoordinate, double yCoordinate);

#endif
