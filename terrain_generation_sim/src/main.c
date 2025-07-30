#include <SDL.h>
#include "terrain_logic_2d.h"
#include "terrain_logic_3d.h"


const int WINDOW_HEIGHT = 800;
const int WINDOW_WIDTH = 800;
const double FEATURE_SCALE_2D = 10.0;
const double FEATURE_SCALE_3D = 10.0;
const double Z_COORDINATE_OFFSET = 0.0;
const double CAMERA_X = FEATURE_SCALE_3D / 2.0;
const double CAMERA_Y = FEATURE_SCALE_3D / 2.0 - 9.0;
const double CAMERA_Z = -10.0;
const double CAMERA_PITCH = 75.0;
const double FOV = 90.0;

typedef enum {
    MODE_2D,
    MODE_3D
} SimulationMode;

SimulationMode currentSimulationMode = MODE_2D;

int main(int argc, char* argv[]) {

    (void)argc;
    (void)argv;

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    window = SDL_CreateWindow(
        "Terrain Generator",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL) {
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (renderer == NULL) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Event e;
    int quit = 0;

    while(!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_1:
                        currentSimulationMode = MODE_2D;
                        printf("Switched to 2D Terrain Generation Mode\n");
                        break;
                    case SDLK_2:
                        currentSimulationMode = MODE_3D;
                        printf("Switched to 3D Terrain Generation Mode\n");
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(renderer);

        if (currentSimulationMode == MODE_2D) {
            for (int y = 0; y < WINDOW_HEIGHT; ++y) {
                for (int x = 0; x < WINDOW_WIDTH; ++x) {

                    double normalizedX = (double)x / WINDOW_WIDTH * FEATURE_SCALE_2D;
                    double normalizedY = (double)y / WINDOW_HEIGHT * FEATURE_SCALE_2D;

                    double noiseValue = perlin_noise_2d(normalizedX, normalizedY);

                    Color pixelColor = get_terrain_color(noiseValue);

                    SDL_SetRenderDrawColor(renderer, pixelColor.r, pixelColor.g, pixelColor.b, 0xFF);
                    SDL_RenderDrawPoint(renderer, x, y);
                }
            }
        } else if (currentSimulationMode == MODE_3D) {

            const double STEP_SIZE_X = 0.1;
            const double STEP_SIZE_Y = 0.1;

            for (double y = 0; y < FEATURE_SCALE_3D; y += STEP_SIZE_Y) {
                for (double x = 0; x < FEATURE_SCALE_3D; x += STEP_SIZE_X) {

                    double noiseValue = perlin_noise_3d(x, y, Z_COORDINATE_OFFSET);

                    double zCoord = noiseValue * 2.0;

                    Point3D terrainPoint = {x, y, zCoord};

                    SDL_Point projectedScreenPoint = project_point(terrainPoint, CAMERA_X, CAMERA_Y, CAMERA_Z, CAMERA_PITCH, FOV, WINDOW_WIDTH, WINDOW_HEIGHT);

                    Color pixelColor = get_terrain_color(noiseValue);

                    SDL_SetRenderDrawColor(renderer, pixelColor.r, pixelColor.g, pixelColor.b, 0xFF);
                    SDL_RenderDrawPoint(renderer, projectedScreenPoint.x, projectedScreenPoint.y);
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}