#ifndef TERRAIN_LOGIC_2D_H
#define TERRAIN_LOGIC_2D_H

#include <SDL.h>
#include <math.h>
#include <stdint.h>


// Hard coded permutation table
extern const uint8_t permutationTable[];

// 2D array which holds gradient vectors
extern const double gradientVectors[16][2];

// Terrain Height Thresholds
#define WATER_LEVEL_THRESHOLD 0.4
#define SAND_LEVEL_THRESHOLD 0.45
#define GRASS_LEVEL_THRESHOLD 0.7
#define ROCK_LEVEL_THRESHOLD 0.85

// Water color characteristics
#define COLOR_WATER_R 0
#define COLOR_WATER_G 0
#define COLOR_WATER_B_MIN 100
#define COLOR_WATER_B_RANGE 155

// Sand color characteristics
#define COLOR_SAND_R_FACTOR 244
#define COLOR_SAND_G_FACTOR 164
#define COLOR_SAND_B_FACTOR 96

// Grass color characteristics
#define COLOR_GRASS_R_FACTOR 70
#define COLOR_GRASS_G_FACTOR 180
#define COLOR_GRASS_B_FACTOR 70

// Rock color characteristics
#define COLOR_ROCK_R_FACTOR 150
#define COLOR_ROCK_G_FACTOR 150
#define COLOR_ROCK_B_FACTOR 150

// Snow color characteristics
#define COLOR_SNOW_R_FACTOR 255
#define COLOR_SNOW_G_FACTOR 255
#define COLOR_SNOW_B_FACTOR 255

// Structure to hold RGB values
typedef struct Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

// Calculates dot product of a selected gradient vector and distance vector
double gradient_dot_product(uint8_t permutationHash, double distanceX, double distanceY);

// Generates a 2D Perlin noise value for x and y
double perlin_noise_2d(double xCoordinate, double yCoordinate);

// Creates and returns color values based on the Perlin noise values
Color get_terrain_color(double noiseValue);

#endif
