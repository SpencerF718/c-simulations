#include "terrain_logic.h"


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

const double gradientVectors[16][2] =  {
    {1, 1}, {-1, 1}, {1, -1}, {-1, -1},
    {1, 0}, {-1, 0}, {0, 1}, {0, -1},
    {1, 1}, {-1, 1}, {1, -1}, {-1, -1},
    {1, 0}, {-1, 0}, {0, 1}, {0, -1}
};

double perlin_fade(double interpolationFactor) {
    double t = interpolationFactor; 
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double gradient_dot_product(uint8_t permutationHash, double distanceX, double distanceY) {
    // Bitwise optimization. Same as % 16
    int index = permutationHash & 0xF;
    return gradientVectors[index][0] * distanceX + gradientVectors[index][1] * distanceY;
}

double perlin_noise_2d(double xCoordinate, double yCoordinate) {
    int floorX = (int)floor(xCoordinate);
    int floorY = (int)floor(yCoordinate);

    double fractionalX = xCoordinate - floorX; 
    double fractionalY = yCoordinate - floorY;

    double fadedX = perlin_fade(fractionalX);
    double fadedY = perlin_fade(fractionalY);

    uint8_t hashAA = permutationTable[permutationTable[floorX & 255] + (floorY & 255)] & 255;
    uint8_t hashBA = permutationTable[permutationTable[(floorX + 1) & 255] + (floorY & 255)] & 255;
    uint8_t hashAB = permutationTable[permutationTable[floorX & 255] + ((floorY + 1) & 255)] & 255;
    uint8_t hashBB = permutationTable[permutationTable[(floorX + 1) & 255] + ((floorY + 1) & 255)] & 255;

    double dotProductAA = gradient_dot_product(hashAA, fractionalX, fractionalY);
    double dotProductBA = gradient_dot_product(hashBA, fractionalX - 1, fractionalY);
    double dotProductAB = gradient_dot_product(hashAB, fractionalX, fractionalY - 1);
    double dotProductBB = gradient_dot_product(hashBB, fractionalX - 1, fractionalY - 1);

    double interpolatedXBottom = dotProductAA + fadedX * (dotProductBA - dotProductAA);
    double interpolatedXTop = dotProductAB + fadedX * (dotProductBB - dotProductAB);

    double finalNoiseValue = interpolatedXBottom + fadedY * (interpolatedXTop - interpolatedXBottom);

    return finalNoiseValue;
}

Color get_terrain_color(double noiseValue) {
    Color color;
    double normalizedNoise = (noiseValue + 1.0) * 0.5;

    if (normalizedNoise < WATER_LEVEL_THRESHOLD) {

        color.r = COLOR_WATER_R;
        color.g = COLOR_WATER_G;
        color.b = (uint8_t)(COLOR_WATER_B_MIN + normalizedNoise * COLOR_WATER_B_RANGE);

    } else if (normalizedNoise < SAND_LEVEL_THRESHOLD) {

        color.r = (uint8_t)(COLOR_SAND_R_FACTOR * normalizedNoise);
        color.g = (uint8_t)(COLOR_SAND_G_FACTOR * normalizedNoise);
        color.b = (uint8_t)(COLOR_SAND_B_FACTOR * normalizedNoise);

    } else if (normalizedNoise < GRASS_LEVEL_THRESHOLD) {

        color.r = (uint8_t)(COLOR_GRASS_R_FACTOR * normalizedNoise);
        color.g = (uint8_t)(COLOR_GRASS_G_FACTOR * normalizedNoise);
        color.b = (uint8_t)(COLOR_GRASS_B_FACTOR * normalizedNoise);

    } else if (normalizedNoise < ROCK_LEVEL_THRESHOLD) {

        color.r = (uint8_t)(COLOR_ROCK_R_FACTOR * normalizedNoise);
        color.g = (uint8_t)(COLOR_ROCK_G_FACTOR * normalizedNoise);
        color.b = (uint8_t)(COLOR_ROCK_B_FACTOR * normalizedNoise);

    } else {

        color.r = (uint8_t)(COLOR_SNOW_R_FACTOR * normalizedNoise);
        color.g = (uint8_t)(COLOR_SNOW_G_FACTOR * normalizedNoise);
        color.b = (uint8_t)(COLOR_SNOW_B_FACTOR * normalizedNoise);
        
    }

    return color;
}