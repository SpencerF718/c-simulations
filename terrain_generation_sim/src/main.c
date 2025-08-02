#include <SDL.h>
#include "terrain_logic_2d.h"
#include "terrain_logic_3d.h"


const int WINDOW_HEIGHT = 800;
const int WINDOW_WIDTH = 800;

const double FEATURE_SCALE_2D = 10.0;
const double FEATURE_SCALE_3D = 10.0;

const double Z_COORDINATE_OFFSET = 0.0;

static double cameraX = FEATURE_SCALE_3D / 2.0;
static double cameraY = 5.0;
static double cameraZ = -10.0;
static double cameraPitch = 0.0;
static double cameraYaw = 0.0;
const double FOV = 90.0;

const double CAMERA_MOVE_SPEED = 0.5;
const double CAMERA_ROTATION_SPEED = 2.0;

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
                    case SDLK_w:
                        if (currentSimulationMode == MODE_3D) {
                            cameraY -= CAMERA_MOVE_SPEED * cos(cameraPitch * M_PI / 180.0);
                            cameraZ -= CAMERA_MOVE_SPEED * sin(cameraPitch * M_PI / 180.0);
                        }
                        break;
                    case SDLK_s:
                        if (currentSimulationMode == MODE_3D) {
                            cameraY += CAMERA_MOVE_SPEED * cos(cameraPitch * M_PI / 180.0);
                            cameraZ += CAMERA_MOVE_SPEED * sin(cameraPitch * M_PI / 180.0);
                        }
                        break;
                    case SDLK_a:
                        if (currentSimulationMode == MODE_3D) {
                            cameraX -= CAMERA_MOVE_SPEED;
                        }
                        break;
                    case SDLK_d:
                        if (currentSimulationMode == MODE_3D) {
                            cameraX += CAMERA_MOVE_SPEED;
                        }
                        break;
                    case SDLK_UP:
                        if (currentSimulationMode == MODE_3D) {
                            cameraPitch += CAMERA_ROTATION_SPEED;
                        }
                        break;
                    case SDLK_DOWN:
                        if (currentSimulationMode == MODE_3D) {
                            cameraPitch -= CAMERA_ROTATION_SPEED;
                        }
                        break;
                    case SDLK_LEFT:
                        if (currentSimulationMode == MODE_3D) {
                            cameraYaw -= CAMERA_ROTATION_SPEED;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (currentSimulationMode == MODE_3D) {
                            cameraYaw += CAMERA_ROTATION_SPEED;
                        }
                        break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(renderer);

        if (currentSimulationMode == MODE_2D) {
            render_2d_terrain(renderer, FEATURE_SCALE_2D, WINDOW_WIDTH, WINDOW_HEIGHT);
        } else if (currentSimulationMode == MODE_3D) {
            render_3d_terrain(renderer, FEATURE_SCALE_3D, Z_COORDINATE_OFFSET, cameraX, cameraY, cameraZ, cameraPitch, cameraYaw, FOV, WINDOW_WIDTH, WINDOW_HEIGHT);
        }
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}