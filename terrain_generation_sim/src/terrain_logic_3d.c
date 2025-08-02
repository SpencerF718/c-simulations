#include "terrain_logic_2d.h"
#include "terrain_logic_3d.h"

const double gradientVectors3D[16][3] = {
    {1, 1, 0}, {-1, 1, 0}, {1, -1, 0}, {-1, -1, 0},
    {1, 0, 1}, {-1, 0, 1}, {1, 0, -1}, {-1, 0, -1},
    {0, 1, 1}, {0, -1, 1}, {0, 1, -1}, {0, -1, -1},
    {1, 1, 0}, {-1, 1, 0}, {0, -1, 1}, {0, -1, -1}
};

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

/*
   Implementation of "smootherstep" function
            6t^5 - 15t^4 + 10t^3 
        =   t^3(6t^2 - 15t + 10)
        =   t * t * t * (t * (t * 6 - 15) + 10)
*/
static double perlin_fade(double interpolationFactor) {
    double t = interpolationFactor;
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double gradient_dot_product_3d(uint8_t permutationHash, double distanceX, double distanceY, double distanceZ) {
    // Bitwise optimization. Same as % 16
    int index = permutationHash & 0xF;
    return gradientVectors3D[index][0] * distanceX + 
           gradientVectors3D[index][1] * distanceY + 
           gradientVectors3D[index][2] * distanceZ;
}

double perlin_noise_3d(double x, double y, double z) {
    int floorX = (int)floor(x);
    int floorY = (int)floor(y);
    int floorZ = (int)floor(z);

    double fractionalX = x - floorX;
    double fractionalY = y - floorY;
    double fractionalZ = z - floorZ;

    double fadedX = perlin_fade(fractionalX);
    double fadedY = perlin_fade(fractionalY);
    double fadedZ = perlin_fade(fractionalZ);

    uint8_t A = permutationTable[floorX & 255] + (floorY & 255);
    uint8_t AA = permutationTable[A & 255] + (floorZ & 255);
    uint8_t AB = permutationTable[(A + 1) & 255] + (floorZ & 255);

    uint8_t B = permutationTable[(floorX + 1) & 255] + (floorY & 255);
    uint8_t BA = permutationTable[B & 255] + (floorZ & 255);
    uint8_t BB = permutationTable[(B + 1) & 255] + (floorZ & 255);

    double dotProductAAA = gradient_dot_product_3d(permutationTable[AA], fractionalX, fractionalY, fractionalZ);
    double dotProductBAA = gradient_dot_product_3d(permutationTable[BA], fractionalX - 1, fractionalY, fractionalZ);
    double dotProductABA = gradient_dot_product_3d(permutationTable[AB], fractionalX, fractionalY - 1, fractionalZ);
    double dotProductBBA = gradient_dot_product_3d(permutationTable[BB], fractionalX - 1, fractionalY - 1, fractionalZ);

    uint8_t hashAPlus1 = permutationTable[floorX & 255] + ((floorY + 1) & 255);
    uint8_t hashAAPlus1 = permutationTable[hashAPlus1 & 255] + ((floorZ + 1) & 255);
    uint8_t hashABPlus1 = permutationTable[(hashAPlus1 + 1) & 255] + ((floorZ + 1) & 255);

    uint8_t hashBPlus1 = permutationTable[(floorX + 1) & 255] + ((floorY + 1) & 255);
    uint8_t hashBAPlus1 = permutationTable[hashBPlus1 & 255] + ((floorZ + 1) & 255);
    uint8_t hashBBPlus1 = permutationTable[(hashBPlus1 + 1) & 255] + ((floorZ + 1) & 255);

    double dotProductAAB = gradient_dot_product_3d(permutationTable[hashAAPlus1], fractionalX, fractionalY, fractionalZ - 1);
    double dotProductBAB = gradient_dot_product_3d(permutationTable[hashBAPlus1], fractionalX - 1, fractionalY, fractionalZ - 1);
    double dotProductABB = gradient_dot_product_3d(permutationTable[hashABPlus1], fractionalX, fractionalY - 1, fractionalZ - 1);
    double dotProductBBB = gradient_dot_product_3d(permutationTable[hashBBPlus1], fractionalX - 1, fractionalY - 1, fractionalZ - 1);

    double interpolatedXYBottom = dotProductAAA + fadedX * (dotProductBAA - dotProductAAA);
    double interpolatedXYTop = dotProductABA + fadedX * (dotProductBBA - dotProductABA);
    double interpolatedZBottom = interpolatedXYBottom + fadedY * (interpolatedXYTop - interpolatedXYBottom);

    double interpolatedXYBottomZ1 = dotProductAAB + fadedX * (dotProductBAB - dotProductAAB);
    double interpolatedXYTopZ1 = dotProductABB + fadedX * (dotProductBBB - dotProductABB);
    double interpolatedZTop = interpolatedXYBottomZ1 + fadedY * (interpolatedXYTopZ1 - interpolatedXYBottomZ1);

    double finalNoiseValue = interpolatedZBottom + fadedZ * (interpolatedZTop - interpolatedZBottom);

    return finalNoiseValue;
}

SDL_Point project_point(Point3D point, double cameraX, double cameraY, double cameraZ, double cameraPitch, double cameraYaw, double fov, int windowWidth, int windowHeight) {
    SDL_Point projectedPoint;
    double translatedX = point.x - cameraX;
    double translatedY = point.y - cameraY;
    double translatedZ = point.z - cameraZ;

    double yawRadians = cameraYaw * M_PI / 180.0;
    double rotatedX = translatedX * cos(yawRadians) - translatedZ * sin(yawRadians);
    double rotatedZ = translatedX * sin(yawRadians) + translatedZ * cos(yawRadians);

    translatedX = rotatedX;
    translatedZ = rotatedZ;

    double pitchRadians = cameraPitch * M_PI / 180.0;
    double finalRotatedY = translatedY * cos(pitchRadians) - translatedZ * sin(pitchRadians);
    double finalRotatedZ = translatedY * sin(pitchRadians) + translatedZ * cos(pitchRadians);

    translatedY = finalRotatedY;
    translatedZ = finalRotatedZ;

    if (translatedZ <= 0.01) {
        translatedZ = 0.01;
    }

    double focalLength = (windowWidth / 2.0) / tan(fov * 0.5 * M_PI / 180.0);
    projectedPoint.x = (int)((translatedX * focalLength / translatedZ) + windowWidth / 2);
    projectedPoint.y = (int)((-translatedY * focalLength / translatedZ) + windowHeight / 2);

    return projectedPoint;
}

void render_3d_terrain(SDL_Renderer* renderer, double featureScale3D, double zCoordinateOffset, double cameraX, double cameraY, double cameraZ, double cameraPitch, double cameraYaw, double fov, int windowWidth, int windowHeight) {
    for (double currentY = 0; currentY < featureScale3D - TERRAIN_3D_STEP_SIZE; currentY += TERRAIN_3D_STEP_SIZE) {
        for (double currentX = 0; currentX < featureScale3D - TERRAIN_3D_STEP_SIZE; currentX += TERRAIN_3D_STEP_SIZE) {

            double noiseValueP00 = perlin_noise_3d(currentX, currentY, zCoordinateOffset);
            double noiseValueP10 = perlin_noise_3d(currentX + TERRAIN_3D_STEP_SIZE, currentY, zCoordinateOffset);
            double noiseValueP01 = perlin_noise_3d(currentX, currentY + TERRAIN_3D_STEP_SIZE, zCoordinateOffset);
            double noiseValueP11 = perlin_noise_3d(currentX + TERRAIN_3D_STEP_SIZE, currentY + TERRAIN_3D_STEP_SIZE, zCoordinateOffset);

            double zCoordP00 = noiseValueP00 * 2.0;
            double zCoordP10 = noiseValueP10 * 2.0;
            double zCoordP01 = noiseValueP01 * 2.0;
            double zCoordP11 = noiseValueP11 * 2.0;

            Point3D point3DP00 = {currentX, currentY, zCoordP00};
            Point3D point3DP10 = {currentX + TERRAIN_3D_STEP_SIZE, currentY, zCoordP10};
            Point3D point3DP01 = {currentX, currentY + TERRAIN_3D_STEP_SIZE, zCoordP01};
            Point3D point3DP11 = {currentX + TERRAIN_3D_STEP_SIZE, currentY + TERRAIN_3D_STEP_SIZE, zCoordP11};

            SDL_Point projectedScreenPointP00 = project_point(point3DP00, cameraX, cameraY, cameraZ, cameraPitch, cameraYaw, fov, windowWidth, windowHeight);
            SDL_Point projectedScreenPointP10 = project_point(point3DP10, cameraX, cameraY, cameraZ, cameraPitch, cameraYaw, fov, windowWidth, windowHeight);
            SDL_Point projectedScreenPointP01 = project_point(point3DP01, cameraX, cameraY, cameraZ, cameraPitch, cameraYaw, fov, windowWidth, windowHeight);
            SDL_Point projectedScreenPointP11 = project_point(point3DP11, cameraX, cameraY, cameraZ, cameraPitch, cameraYaw, fov, windowWidth, windowHeight);

            Color pixelColor = get_terrain_color(noiseValueP00);
            SDL_SetRenderDrawColor(renderer, pixelColor.r, pixelColor.g, pixelColor.b, 0xFF);

            SDL_RenderDrawLine(renderer, projectedScreenPointP00.x, projectedScreenPointP00.y, projectedScreenPointP10.x, projectedScreenPointP10.y);
            SDL_RenderDrawLine(renderer, projectedScreenPointP10.x, projectedScreenPointP10.y, projectedScreenPointP01.x, projectedScreenPointP01.y);
            SDL_RenderDrawLine(renderer, projectedScreenPointP01.x, projectedScreenPointP01.y, projectedScreenPointP00.x, projectedScreenPointP00.y);

            SDL_RenderDrawLine(renderer, projectedScreenPointP10.x, projectedScreenPointP10.y, projectedScreenPointP11.x, projectedScreenPointP11.y);
            SDL_RenderDrawLine(renderer, projectedScreenPointP11.x, projectedScreenPointP11.y, projectedScreenPointP01.x, projectedScreenPointP01.y);
        }
    }
}