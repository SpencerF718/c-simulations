#include "fluid_logic.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


Fluid* fluid_init(float density, int numX, int numY, float cellSize) {

    Fluid* fluid = (Fluid*)calloc(1, sizeof(Fluid));
    if (fluid == NULL) {
        printf("ERROR: fluid_init failed to allocate Fluids struct");
        return NULL;
    }

    fluid->density = density;

    // include boundary cells
    fluid->numCellsX = numX + 2;
    fluid->numCellsY = numY + 2;

    fluid->totalNumCells = (size_t)fluid->numCellsX * fluid->numCellsY;
    fluid->cellSize = cellSize;

    // allocate memory
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
        printf("ERROR: fluid_init failed to allocate fluid arrays\n");
        return NULL;
    }

    return fluid;
}

void fluid_free(Fluid* fluidPtr) {
    if (fluidPtr) {
        free(fluidPtr->velocityX);
        free(fluidPtr->velocityY);
        free(fluidPtr->newVelocityX);
        free(fluidPtr->newVelocityY);
        free(fluidPtr->pressure);
        free(fluidPtr->solidFlags);
        free(fluidPtr->smokeDensity);
        free(fluidPtr->newSmokeDensity);
        free(fluidPtr);
    }
}

void fluid_integrate(Fluid* fluidPtr, float deltaTime, float gravityForce) {

    int numRows = fluidPtr->numCellsY;

    for (int i = 1; i < fluidPtr->numCellsX - 1; ++i) {
        for (int j = 1; j < fluidPtr->numCellsY - 1; ++j) {
            size_t currentCellIndex = (size_t)i * numRows + j;

            // Apply gravity to only fluid cells
            if (fluidPtr->solidFlags[currentCellIndex] == 1.0f) {
                fluidPtr->velocityY[currentCellIndex] += gravityForce * deltaTime;
            }
        }
    }
}

void fluid_solve_incompressibility(Fluid* fluidPtr, int numIterations, float overRelaxation) {

    int numRows = fluidPtr->numCellsY;

    for (int iter = 0; iter < numIterations; ++iter) {
        for (int i = 1; i < fluidPtr->numCellsX - 1; ++i) {
            for (int j = 1; j < fluidPtr->numCellsY - 1; ++j) {
                size_t currentCellIndex = (size_t)i * numRows + j;

                if (fluidPtr->solidFlags[currentCellIndex] == 0.0f) continue;

                // indicate neighboring solid cells
                float sx0 = fluidPtr->solidFlags[(size_t)(i - 1) * numRows + j];
                float sx1 = fluidPtr->solidFlags[(size_t)(i + 1) * numRows + j];
                float sy0 = fluidPtr->solidFlags[(size_t)i * numRows + (j - 1)];
                float sy1 = fluidPtr->solidFlags[(size_t)i * numRows + (j + 1)];
                float s = sx0 + sx1 + sy0 + sy1;

                if (s == 0.0f) continue;

                // calculate velocity divergence
                float divergence = fluidPtr->velocityX[currentCellIndex] - fluidPtr->velocityX[(size_t)(i + 1) * numRows + j] +
                                   fluidPtr->velocityY[currentCellIndex] - fluidPtr->velocityY[(size_t)i * numRows + (j + 1)];

                // calculate pressure change
                float dp = -divergence / s;

                // apply over relaxation
                dp *= overRelaxation;

                fluidPtr->pressure[currentCellIndex] += dp;

                // adjust velocities based on changes
                fluidPtr->velocityX[currentCellIndex] += sx0 * dp;
                fluidPtr->velocityX[(size_t)(i + 1) * numRows + j] -= sx1 * dp;
                fluidPtr->velocityY[currentCellIndex] += sy0 * dp;
                fluidPtr->velocityY[(size_t)i * numRows + (j + 1)] -= sy1 * dp;
            }
        }
    }
}


void fluid_extrapolate(Fluid* fluidPtr) {
    int numRows = fluidPtr->numCellsY;

    for (int i = 0; i < fluidPtr->numCellsX; ++i) {
        for (int j = 0; j < fluidPtr->numCellsY; ++j) {
            size_t currentCellIndex = (size_t)i * numRows + j;
            // set velocity for solid cells
            if (fluidPtr->solidFlags[currentCellIndex] == 0.0f) { 
                fluidPtr->velocityX[currentCellIndex] = 0.0f;
                fluidPtr->velocityY[currentCellIndex] = 0.0f;
            }
        }
    }
}


float fluid_sample_field(Fluid* fluidPtr, float xPos, float yPos, FieldType fieldType) {
    float cellSize = fluidPtr->cellSize;
    int numRows = fluidPtr->numCellsY;

    // convert world coordinates to cell coordinates
    float cellX = xPos / cellSize;
    float cellY = yPos / cellSize;

    int i = (int)cellX;
    int j = (int)cellY;
    float fx = cellX - i;
    float fy = cellY - j;

    // clamp cell indices to grid boundary
    i = fmaxf(0, fminf(i, fluidPtr->numCellsX - 1));
    j = fmaxf(0, fminf(j, fluidPtr->numCellsY - 1));

    // get indices for nearest grid points
    int i0 = i;
    int i1 = fminf(i + 1, fluidPtr->numCellsX - 1); 
    int j0 = j;
    int j1 = fminf(j + 1, fluidPtr->numCellsY - 1);

    // indices for 4 nearest grid points
    size_t idx00 = (size_t)i0 * numRows + j0;
    size_t idx10 = (size_t)i1 * numRows + j0;
    size_t idx01 = (size_t)i0 * numRows + j1;
    size_t idx11 = (size_t)i1 * numRows + j1;

    float val00, val10, val01, val11;

    // select the correct field based on fieldType
    switch (fieldType) {
        case U_FIELD:
            val00 = fluidPtr->velocityX[idx00];
            val10 = fluidPtr->velocityX[idx10];
            val01 = fluidPtr->velocityX[idx01];
            val11 = fluidPtr->velocityX[idx11];
            break;
        case V_FIELD:
            val00 = fluidPtr->velocityY[idx00];
            val10 = fluidPtr->velocityY[idx10];
            val01 = fluidPtr->velocityY[idx01];
            val11 = fluidPtr->velocityY[idx11];
            break;
        case SMOKE_FIELD:
            val00 = fluidPtr->smokeDensity[idx00];
            val10 = fluidPtr->smokeDensity[idx10];
            val01 = fluidPtr->smokeDensity[idx01];
            val11 = fluidPtr->smokeDensity[idx11];
            break;
        default:
            return 0.0f;
    }

    // bilinear interpolation
    float result = (val00 * (1 - fx) * (1 - fy)) +
                   (val10 * fx * (1 - fy)) +
                   (val01 * (1 - fx) * fy) +
                   (val11 * fx * fy);
    return result;
}

void fluid_advect_velocity(Fluid* fluidPtr, float deltaTime) {

    float cellSize = fluidPtr->cellSize;
    float halfCellSize = cellSize * 0.5f;
    int numRows = fluidPtr->numCellsY;

    // copy velocities for data in advection
    memcpy(fluidPtr->newVelocityX, fluidPtr->velocityX, fluidPtr->totalNumCells * sizeof(float));
    memcpy(fluidPtr->newVelocityY, fluidPtr->velocityY, fluidPtr->totalNumCells * sizeof(float));

    for (int i = 1; i < fluidPtr->numCellsX - 1; ++i) {
        for (int j = 1; j < fluidPtr->numCellsY - 1; ++j) {
            size_t currentCellIndex = (size_t)i * numRows + j;
            if (fluidPtr->solidFlags[currentCellIndex] == 0.0f) continue; 

            // advect x-velocity
            float xCurrent = (float)i * cellSize; 
            float yCurrent = (float)j * cellSize + halfCellSize;
            float uAvg = fluid_sample_field(fluidPtr, xCurrent, yCurrent, U_FIELD);
            float vAvg = fluid_sample_field(fluidPtr, xCurrent, yCurrent, V_FIELD);
            float prevX = xCurrent - deltaTime * uAvg;
            float prevY = yCurrent - deltaTime * vAvg;
            fluidPtr->newVelocityX[currentCellIndex] = fluid_sample_field(fluidPtr, prevX, prevY, U_FIELD);

            // advect y-velocity
            xCurrent = (float)i * cellSize + halfCellSize;
            yCurrent = (float)j * cellSize;
            uAvg = fluid_sample_field(fluidPtr, xCurrent, yCurrent, U_FIELD);
            vAvg = fluid_sample_field(fluidPtr, xCurrent, yCurrent, V_FIELD);
            prevX = xCurrent - deltaTime * uAvg;
            prevY = yCurrent - deltaTime * vAvg;
            fluidPtr->newVelocityY[currentCellIndex] = fluid_sample_field(fluidPtr, prevX, prevY, V_FIELD);
        }
    }
    // update velocity with advected values
    memcpy(fluidPtr->velocityX, fluidPtr->newVelocityX, fluidPtr->totalNumCells * sizeof(float));
    memcpy(fluidPtr->velocityY, fluidPtr->newVelocityY, fluidPtr->totalNumCells * sizeof(float));
}

void fluid_advect_smoke(Fluid* fluidPtr, float deltaTime) {
    float cellSize = fluidPtr->cellSize;
    float halfCellSize = cellSize * 0.5f;
    int numRows = fluidPtr->numCellsY;

    // copy smoke density for data during advection
    memcpy(fluidPtr->newSmokeDensity, fluidPtr->smokeDensity, fluidPtr->totalNumCells * sizeof(float));

    for (int i = 1; i < fluidPtr->numCellsX - 1; ++i) {
        for (int j = 1; j < fluidPtr->numCellsY - 1; ++j) {
            size_t currentCellIndex = (size_t)i * numRows + j;
            // solid cells don't have smoke
            if (fluidPtr->solidFlags[currentCellIndex] == 0.0f) {
                fluidPtr->newSmokeDensity[currentCellIndex] = 0.0f;
                continue;
            }

            // cell center coordinates
            float xCurrent = (float)i * cellSize + halfCellSize;
            float yCurrent = (float)j * cellSize + halfCellSize;

            float uAvg = fluid_sample_field(fluidPtr, xCurrent, yCurrent, U_FIELD);
            float vAvg = fluid_sample_field(fluidPtr, xCurrent, yCurrent, V_FIELD);

            float prevX = xCurrent - deltaTime * uAvg;
            float prevY = yCurrent - deltaTime * vAvg;

            fluidPtr->newSmokeDensity[currentCellIndex] = fluid_sample_field(fluidPtr, prevX, prevY, SMOKE_FIELD);
        }
    }
    // update smoke density with advected values
    memcpy(fluidPtr->smokeDensity, fluidPtr->newSmokeDensity, fluidPtr->totalNumCells * sizeof(float));
}

void fluid_set_obstacle(Fluid* fluidPtr, int x, int y, float isSolidFlag) {

        if (x < 0 || x >= fluidPtr->numCellsX || y < 0 || y >= fluidPtr->numCellsY) { 
        return; 
    }

    int numRows = fluidPtr->numCellsY;
    size_t cellIndex = (size_t)x * numRows + y;

    fluidPtr->solidFlags[cellIndex] = isSolidFlag;

    // if solid, make it static
    if (isSolidFlag == 0.0f) {
        fluidPtr->smokeDensity[cellIndex] = 0.0f; 
        fluidPtr->velocityX[cellIndex] = 0.0f; 
        if (x + 1 < fluidPtr->numCellsX) fluidPtr->velocityX[(size_t)(x + 1) * numRows + y] = 0.0f; 
        fluidPtr->velocityY[cellIndex] = 0.0f;
        if (y + 1 < fluidPtr->numCellsY) fluidPtr->velocityY[(size_t)x * numRows + (y + 1)] = 0.0f;
    }
}

void fluid_simulate_step(Fluid* fluidPtr, int numIterations, float deltaTime, float gravityForce, float overRelaxation, float dissipation, float smokeDissipation) {
    size_t totalNumCells = fluidPtr->totalNumCells;

    // apply velocity dissipation
    for (size_t i = 0; i < totalNumCells; ++i) {
        fluidPtr->velocityX[i] *= dissipation;
        fluidPtr->velocityY[i] *= dissipation;
    }

    // apply smoke dissipation
    for (size_t i = 0; i < totalNumCells; ++i) {
        fluidPtr->smokeDensity[i] *= smokeDissipation;
    }

    fluid_integrate(fluidPtr, deltaTime, gravityForce);
    fluid_solve_incompressibility(fluidPtr, numIterations, overRelaxation);
    fluid_extrapolate(fluidPtr);
    fluid_advect_velocity(fluidPtr, deltaTime);
    fluid_advect_smoke(fluidPtr, deltaTime);
}
