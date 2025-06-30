#ifndef FLUID_LOGIC_H
#define FLUID_LOGIC_H

#include <stddef.h>

/**
 * struct that holds grid-based data for Eulerian fluid simulation.
 */
typedef struct {
    
    int numCellsX;
    int numCellsY;
    size_t totalNumCells;
    float cellSize;
    float density;

    float* velocityX;
    float* velocityY;
    float* newVelocityX;
    float* newVelocityY;
    float* pressure;
    float* smokeDensity;
    float* newSmokeDensity;
    float* solidFlags;

} Fluid;

/**
 * Initializes a new Fluid simulation.
 */
Fluid* fluid_init(float density, int numX, int numY, float cellSize);

/**
 * Frees memory for Fluid struct
 */
void fluid_free(Fluid* fluidPtr);


/**
 * Applies gravity to fluid's velocity field
 */
void fluid_integrate(Fluid* fluid, float deltaTime, float gravityForce);

#endif 