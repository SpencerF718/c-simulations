#include "fluid_logic.h"
#include <stdlib.h>
#include <string.h>

Fluid* fluid_init(float density, int numX, int numY, float cellSize) {

    Fluid* fluid = (Fluid*)calloc(1, sizeof(Fluid));
    if (fluid == NULL) {
        printf("ERROR: fluid_init failed to allocate Fluids struct");
        return NULL;
    }

    fluid->density = density;
    fluid->numCellsX = numX + 2;
    fluid->numCellsY = numY + 2;
    fluid->totalNumCells = (size_t)fluid->numCellsX * fluid->numCellsY;
    fluid->cellSize = cellSize;

    fluid->velocityX = (float*)calloc(fluid->totalNumCells, sizeof(float));
    fluid->velocityY = (float*)calloc(fluid->totalNumCells, sizeof(float));
    fluid->newVelocityX = (float*)calloc(fluid->totalNumCells, sizeof(float));
    fluid->newVelocityY = (float*)calloc(fluid->totalNumCells, sizeof(float));
    fluid->pressure = (float*)calloc(fluid->totalNumCells, sizeof(float));
    fluid->solidFlags = (float*)calloc(fluid->totalNumCells, sizeof(float));
    fluid->smokeDensity = (float*)calloc(fluid->totalNumCells, sizeof(float));
    fluid->newSmokeDensity = (float*)calloc(fluid->totalNumCells, sizeof(float));

    if (!fluid->velocityX || !fluid->velocityY || !fluid->newVelocityX || !fluid->newVelocityY ||
        !fluid->pressure || !fluid->solidFlags || !fluid->smokeDensity || !fluid->newSmokeDensity) {
        fluid_free(fluid);
        return NULL; 
    }

    for (size_t i = 0; i < fluid->totalNumCells; i++) {
        fluid->smokeDensity[i] = 1.0f;
    }

    return fluid;

}

void fluid_free(Fluid* fluid_ptr) {

    if (fluid_ptr == NULL) {
        return;
    }

    free(fluid_ptr->velocityX);
    free(fluid_ptr->velocityY);
    free(fluid_ptr->newVelocityX);
    free(fluid_ptr->newVelocityY);
    free(fluid_ptr->pressure);
    free(fluid_ptr->solidFlags);
    free(fluid_ptr->smokeDensity);
    free(fluid_ptr->newSmokeDensity);

    fluid_ptr->velocityX = NULL;
    fluid_ptr->velocityY = NULL;
    fluid_ptr->newVelocityX = NULL;
    fluid_ptr->newVelocityY = NULL;
    fluid_ptr->pressure = NULL;
    fluid_ptr->solidFlags = NULL;
    fluid_ptr->smokeDensity = NULL;
    fluid_ptr->newSmokeDensity = NULL;

    free(fluid_ptr);    
    
}
