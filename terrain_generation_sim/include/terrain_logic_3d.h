#ifndef TERRAIN_LOGIC_3D_H
#define TERRAIN_LOGIC_3D_H

#include<SDL.h>
#include <stdint.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "terrain_logic_2d.h"

#define TERRAIN_3D_STEP_SIZE 0.25

// Hard coded permutation table
extern const uint8_t permutationTable[];

// Array to hold 3D gradient vectors
extern const double gradientVectors3D[16][3];

// Structure to hold the dimensions of 3D terrain with an array that holds the data
typedef struct {
    int width;
    int height;
    int depth;
    float* data;
} Terrain3D;

// Structure to hold 3D point values
typedef struct {
    double x;
    double y;
    double z;
} Point3D;

// Initialize a new Terrain3D struct
Terrain3D* terrain3d_init(int width, int heigh, int depth);

// Frees the allocated memory for the Terrain3D struct
void terrain3d_free(Terrain3D* terrain);

// Generates 3D perlin noise based on xyz coordinates
double perlin_noise_3d(double x, double y, double z);

// Calculates dot product of a selected gradient vector and 3D distance vector
double gradient_dot_product_3d(uint8_t permutationHash, double distanceX, double distanceY, double distanceZ);

// Projects a 3D point onto a 2D plane
SDL_Point project_point(Point3D point, double cameraX, double cameraY, double cameraZ, double cameraPitch, double cameraYaw, double fov, int windowWidth, int windowHeight);

// Renderes the 3D terrain
void render_3d_terrain(SDL_Renderer* renderer, double featureScale3D, double zCoordinateOffset, double cameraX, double cameraY, double cameraZ, double cameraPitch, double cameraYaw, double fov, int windowWidth, int windowHeight);

#endif
