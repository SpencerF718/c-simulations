#include <SDL.h>
#include <stdio.h>

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


