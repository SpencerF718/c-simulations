#include <SDL.h>
#include <stdio.h>
#include <string.h>

#include "terrain_logic_2d.h"
#include "terrain_logic_3d.h"


const int WINDOW_HEIGHT = 800;
const int WINDOW_WIDTH = 800;
const double FEATURE_SCALE = 10.0;

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
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(renderer);

        for (int y = 0; y < WINDOW_HEIGHT; ++y) {
            for (int x = 0; x < WINDOW_WIDTH; ++x) {

                double normalizedX = (double)x / WINDOW_WIDTH * FEATURE_SCALE;
                double normalizedY = (double)y / WINDOW_HEIGHT * FEATURE_SCALE;

                double noiseValue = perlin_noise_2d(normalizedX, normalizedY);

                Color pixelColor = get_terrain_color(noiseValue);

                SDL_SetRenderDrawColor(renderer, pixelColor.r, pixelColor.g, pixelColor.b, 0xFF);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}