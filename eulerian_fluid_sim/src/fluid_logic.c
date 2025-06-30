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

void fluid_free(Fluid* fluidPtr) {

    if (fluidPtr == NULL) {
        return;
    }

    free(fluidPtr->velocityX);
    free(fluidPtr->velocityY);
    free(fluidPtr->newVelocityX);
    free(fluidPtr->newVelocityY);
    free(fluidPtr->pressure);
    free(fluidPtr->solidFlags);
    free(fluidPtr->smokeDensity);
    free(fluidPtr->newSmokeDensity);

    fluidPtr->velocityX = NULL;
    fluidPtr->velocityY = NULL;
    fluidPtr->newVelocityX = NULL;
    fluidPtr->newVelocityY = NULL;
    fluidPtr->pressure = NULL;
    fluidPtr->solidFlags = NULL;
    fluidPtr->smokeDensity = NULL;
    fluidPtr->newSmokeDensity = NULL;

    free(fluidPtr);    
    
}

void fluid_integrate(Fluid* fluidPtr, float deltaTime, float gravityForce) {

    int numRows = fluidPtr->numCellsY;

    for (int i = 1; i < fluidPtr->numCellsX; i++) {
        for (int j = 1; j < fluidPtr->numCellsY; j++) {

            size_t currentIndex = i * numRows + j;
            size_t cellBelowIndex = i * numRows + (j - 1);

            if (fluidPtr->solidFlags[currentIndex] != 0.0f && fluidPtr->solidFlags[cellBelowIndex] != 0.0f) {
                fluidPtr->velocityY[currentIndex] += gravityForce * deltaTime;
            }
        }
    }
}

void fluid_solve_incompressibility(Fluid* fluidPtr, int numIterations, float deltaTime, float overRelaxation) {

    int numRows = fluidPtr->numCellsY;
    float coefficientPressure = fluidPtr->density * fluidPtr->cellSize / deltaTime;

    for (int n = 0; n < numIterations; n++) {
        for (int i = 1; i < fluidPtr->numCellsX; i++) {
            for (int j = 1; j < fluidPtr->numCellsY; j++) {

                size_t currentCellIndex = i * numRows + j;

                if (fluidPtr->solidFlags[currentCellIndex] == 0.0f) {
                    continue;
                }

                float solidFlagCurrent = fluidPtr->solidFlags[currentCellIndex];
                float solidFlagLeft    = fluidPtr->solidFlags[(i - 1) * numRows + j];
                float solidFlagRight   = fluidPtr->solidFlags[(i + 1) * numRows + j];
                float solidFlagBelow   = fluidPtr->solidFlags[i * numRows + (j - 1)];
                float solidFlagAbove   = fluidPtr->solidFlags[i * numRows + (j + 1)];

                float solidSumNeighbors = solidFlagAbove + solidFlagBelow + solidFlagLeft + solidFlagRight;

                if (solidSumNeighbors == 0.0f) {
                    continue;
                }

                 float divergence = fluidPtr->velocityX[(i + 1) * numRows + j] - fluidPtr->velocityX[currentCellIndex] +
                                    fluidPtr->velocityY[i * numRows + (j + 1)] - fluidPtr->velocityY[currentCellIndex];
                
                float pressureChange = -divergence / solidSumNeighbors;
                pressureChange *= overRelaxation;

                fluidPtr->pressure[currentCellIndex] += coefficientPressure * pressureChange;

                fluidPtr->velocityX[currentCellIndex] -= solidFlagLeft * pressureChange;
                fluidPtr->velocityX[(i + 1) * numRows + j] += solidFlagRight * pressureChange;
                fluidPtr->velocityY[currentCellIndex] -= solidFlagBelow * pressureChange;
                fluidPtr->velocityY[i * numRows + (j + 1)] += solidFlagAbove * pressureChange;

            }
        }
    }
}

void fluid_extrapolate(Fluid* fluidPtr) {

    int numRows = fluidPtr->numCellsY;

    for (int i = 0; i < fluidPtr->numCellsX; i++) {
        fluidPtr->velocityX[i * numRows + 0] = fluidPtr->velocityX[i * numRows + 1];
        fluidPtr->velocityX[i * numRows + (fluidPtr->numCellsY - 1)] = fluidPtr->velocityX[i * numRows + (fluidPtr->numCellsY - 2)];
    }

    for (int j = 0; j < fluidPtr->numCellsY; j++) {
        fluidPtr->velocityY[0 * numRows + j] = fluidPtr->velocityY[1 * numRows + j];
        fluidPtr->velocityY[(fluidPtr->numCellsX - 1) * numRows + j] = fluidPtr->velocityY[(fluidPtr->numCellsX - 2) * numRows + j];
    }
}
