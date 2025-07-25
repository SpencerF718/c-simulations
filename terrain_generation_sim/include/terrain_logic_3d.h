#ifndef TERRAIN_LOGIC_3D_H
#define TERRAIN_LOGIC_3D_H

#include <stdint.h>
#include <math.h>


// Structure to hold the dimensions of 3D terrain with an array that holds the data
typedef struct {
    int width;
    int height;
    int depth;
    float* data;
} Terrain3D;

// Initialize a new Terrain3D struct
Terrain3D* terrain3d_init(int width, int heigh, int depth);

// Frees the allocated memory for the Terrain3D struct
void terrain3d_free(Terrain3D* terrain);

/*
   Implementation of "smootherstep" function
            6t^5 - 15t^4 + 10t^3
        =   t^3(6t^2 - 15t + 10)
        =   t * t * t * (t * (t * 6 - 15) + 10)
*/
double perlin_fade(double interpolationFactor);

#endif
