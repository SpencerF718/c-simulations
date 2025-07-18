#ifndef FLUID_LOGIC_H
#define FLUID_LOGIC_H

#include <stddef.h>


/**
 * Enum for field types used in sampling.
 */
typedef enum {
    U_FIELD,
    V_FIELD,
    SMOKE_FIELD
} FieldType;

/**
 * struct that holds grid-based data for simulation.
 */
typedef struct {
    
    int numCellsX;
    int numCellsY;
    size_t totalNumCells;
    float cellSize;

    float* velocityX;
    float* velocityY;
    float* newVelocityX;
    float* newVelocityY;

    float* pressure;

    float density;
    float* smokeDensity;
    float* newSmokeDensity;

    float* solidFlags;

} Fluid;

/**
 * Initializes a new Fluid simulation.
 */
Fluid* fluid_init(float density, int numX, int numY, float cellSize);

/**
 * Frees memory for Fluid struct.
 */
void fluid_free(Fluid* fluidPtr);


/**
 * Applies gravity to fluid's velocity field.
 */
void fluid_integrate(Fluid* fluid, float deltaTime, float gravityForce);

/**
 * Adjusts variables to account for incompressibility.
 */
void fluid_solve_incompressibility(Fluid* fluidPtr, int numIterations, float overRelaxation);

/**
 * Extrapolates velocities to the edge of the fluid grid.
 */
void fluid_extrapolate(Fluid* fluidPtr);

/**
 * Samples a field (velocity X, velocity Y, or smoke density) at a given position.
 */
float fluid_sample_field(Fluid* fluidPtr, float xPos, float yPos, FieldType fieldType);

/**
 * Advects the fluid's velocity field.
 */
void fluid_advect_velocity(Fluid* fluidPtr, float deltaTime);

/**
 * Advects the smoke density field.
 */
void fluid_advect_smoke(Fluid* fluidPtr, float deltaTime);

/**
 * Sets a cell as an obstacle (solid) or fluid.
 */
void fluid_set_obstacle(Fluid* fluidPtr, int x, int y, float isSolidFlag);

/**
 * Performs one step of the fluid simulation.
 */
void fluid_simulate_step(Fluid* fluid, int numIterations, float deltaTime, float gravityForce, float overRelaxation, float dissipation, float smokeDissipation);

#endif
