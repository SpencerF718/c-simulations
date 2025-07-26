#include "terrain_logic_3d.h"


const uint8_t permutationTable[512] = {
    151,160,137, 91, 90, 15,131, 13,201, 95, 96,155, 68, 11, 27,194,
    140, 83,165,164,127, 73,130, 26, 47,247,208,241,101,114,124, 71,
    88, 28,212, 19,178, 59,209, 21, 58, 40, 24,162,136, 14, 82,107,
    125, 41,161, 48,138,103,234, 60, 57, 56, 72,118, 93,122,174,106,
    99,102,108,187,143, 62,113,237, 76, 17,252,185,167, 87, 49,123,
    149,158,249, 12,246, 64, 46,147, 54, 98,168,172, 30,170, 75,181,
    74, 31, 84,204,176,192,109,242,169, 78,145,200, 39,128,236,251,
    211, 86, 43,233,189,205,255,148,157, 10,216,196,163, 16, 70, 44,
    173,184,104, 38,188, 61,195,183, 92,253, 79,100, 42,175, 55,235,
    177,166, 69,250,154, 94,190, 32,229,198,141,126, 53, 77,153, 50,
    203,139,207,248,228,223, 25, 18,133,117,105, 97,112, 16, 70, 44,
    215, 65, 33,129, 66,220, 95, 96,155, 68, 11, 27,194,140, 83,165,
    149,158,249, 12,246, 64, 46,147, 54, 98,168,172, 30,170, 75,181,
    151,160,137, 91, 90, 15,131, 13,201, 95, 96,155, 68, 11, 27,194,
    215, 65, 33,129, 66,220, 95, 96,155, 68, 11, 27,194,140, 83,165,
    151,160,137, 91, 90, 15,131, 13,201, 95, 96,155, 68, 11, 27,194,
};


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

double perlin_fade(double interpolationFactor) {
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
