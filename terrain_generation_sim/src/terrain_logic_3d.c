#include "terrain_logic_3d.h"
#include <stdio.h>
#include <stdlib.h>

Terrain3D* terrain3d_init(int width, int height, int depth) {
    Terrain3D* terrain = (Terrain3D*)calloc(1, sizeof(Terrain3D));

    if (terrain == NULL) {
        fprintf(stderr, "ERROR: Memory allocation error for Terrain3D\n");
        return NULL;
    }

    terrain->width = width;
    terrain->height = height;
    terrain->depth = depth;

    size_t numElements = (size_t)width * height * depth;

    terrain->data = (float*)calloc(numElements, sizeof(float));

    if (terrain->data == NULL) {
        fprintf(stderr, "ERROR: Memory allocation error for Terrain3D data\n");
        return NULL;
    }

    return terrain;
}

void terrain3d_free(Terrain3D* terrain) {
    if (terrain == NULL) {
        return;
    }

    if (terrain->data != NULL) {
        free(terrain->data);
        terrain->data = NULL;
    }

    free(terrain);
}

double perlin_fade(double interpolationFactor) {
    double t = interpolationFactor;
    return t * t * t * (t * (t * 6 - 15) + 10);
}