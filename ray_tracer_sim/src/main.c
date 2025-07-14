#include <SDL.h>
#include <stdio.h>
#include "ray_logic.h"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

int main(int argc, char* argv[]) {

    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window* sdlWindow = NULL;
    SDL_Renderer* sdlRenderer = NULL;

    sdlWindow = SDL_CreateWindow (
        "Ray Tracer Simulation",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (sdlWindow == NULL) {
        SDL_Quit();
        return 1;
    }

    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (sdlRenderer == NULL) {
        SDL_DestroyWindow(sdlWindow);
        SDL_Quit();
        return 1;
    }

    Vec3 cameraPosition = {0.0, 0.0, 0.0};
    Vec3 cameraLookAt = {0.0, 0.0, -1.0};
    Vec3 cameraUpVector = {0.0, 1.0, 0.0};
    double cameraFov = 90.0;

    Vec3 sphere1Center = {0.0, 0.0, -5.0};
    double sphere1Radius = 1.0;
    Color sphere1Color = {1.0, 0.0, 0.0};
    Sphere redSphere = sphere_create(sphere1Center, sphere1Radius, sphere1Color);

    Vec3 sphere2Center = {2.0, -0.5, -8.0};
    double sphere2Radius = 1.5;
    Color sphere2Color = {0.0, 0.0, 1.0};
    Sphere blueSphere = sphere_create(sphere2Center, sphere2Radius, sphere2Color);


    int quitApplication = 0;
    SDL_Event eventHandler;

    while (!quitApplication) {

        while (SDL_PollEvent(&eventHandler) != 0) {
            if (eventHandler.type == SDL_QUIT) {
                quitApplication = 1;
            }
        }

        SDL_SetRenderDrawColor(sdlRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(sdlRenderer);
        SDL_RenderPresent(sdlRenderer);
    }

    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();

    return 0;
}


