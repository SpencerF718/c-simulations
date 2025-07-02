#include <SDL.h>
#include <stdio.h>
#include <math.h> 

#include "fluid_logic.h" 

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

static int mouseIsDown = 0;
static int prevMouseX = 0;
static int prevMouseY = 0;

void render_fluid(SDL_Renderer* renderer, const Fluid* fluid) {

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
    SDL_RenderClear(renderer);

    float cellWidth = (float)WINDOW_WIDTH / (fluid->numCellsX - 2);
    float cellHeight = (float)WINDOW_HEIGHT / (fluid->numCellsY - 2);

    for (int i = 1; i < fluid->numCellsX - 1; ++i) {
        for (int j = 1; j < fluid->numCellsY - 1; ++j) {

            size_t index = (size_t)i * fluid->numCellsY + j;
            float smoke = fluid->smokeDensity[index];

            // uncomment the following code if you want gray smoke on a white background.
            // Uint8 color = (Uint8)((1.0f - fminf(smoke * 1.5f, 1.0f)) * 255.0f);
            Uint8 color = (Uint8)(fminf(smoke * 255.0f, 255.0f));

            SDL_SetRenderDrawColor(renderer, color, color, color, 255);

            SDL_Rect cellRect = {
                (int)((i - 1) * cellWidth),
                (int)(WINDOW_HEIGHT - ((j - 1) * cellHeight) - cellHeight), 
                (int)(cellWidth + 1),
                (int)(cellHeight + 1)
            };
            SDL_RenderFillRect(renderer, &cellRect);
        }
    }
    SDL_RenderPresent(renderer);
}

int screen_to_fluid_x(int screenX, int numCellsX, int windowWidth) {
    return (int)((float)screenX / windowWidth * (numCellsX - 2)) + 1;
}

int screen_to_fluid_y(int screenY, int numCellsY, int windowHeight) {
    return (int)((float)(windowHeight - screenY) / windowHeight * (numCellsY - 2)) + 1;
}

int main(int argc, char* argv[]) {

    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Eulerian Fluid Simulation",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }


    float density = 1.0f;

    /* increase the value to change the 'resolution' of the smoke (WARNING: EXPENSIVE) 
       recommended value is 100
    */
    int numX = 100; 
    int numY = 100; 

    float cellSize = 1.0f / numY; 

    Fluid* fluid = fluid_init(density, numX, numY, cellSize);
    if (fluid == NULL) {
        printf("Failed to initialize fluid simulation.\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // initialize solid flags and smoke density for cells
    for (int i = 0; i < fluid->numCellsX; ++i) {
        for (int j = 0; j < fluid->numCellsY; ++j) {
            size_t index = (size_t)i * fluid->numCellsY + j;
            // make boundaries obstacles
            if (i == 0 || i == fluid->numCellsX - 1 || j == 0 || j == fluid->numCellsY - 1) {
                fluid_set_obstacle(fluid, i, j, 0.0f); 
            } else {
                fluid_set_obstacle(fluid, i, j, 1.0f); 
                fluid->smokeDensity[index] = 0.0f;
            }
        }
    }
    
    SDL_Event e;
    int quit = 0;
    Uint64 lastTick = SDL_GetPerformanceCounter();

    // main simulation loop
    while (!quit) {
        // event handling
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = 1;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mouseIsDown = 1;
                    prevMouseX = e.button.x;
                    prevMouseY = e.button.y;

                    int fluidGridX = screen_to_fluid_x(e.button.x, fluid->numCellsX, WINDOW_WIDTH);
                    int fluidGridY = screen_to_fluid_y(e.button.y, fluid->numCellsY, WINDOW_HEIGHT);

                    fluidGridX = fmaxf(1, fminf(fluidGridX, fluid->numCellsX - 2));
                    fluidGridY = fmaxf(1, fminf(fluidGridY, fluid->numCellsY - 2));

                    size_t fluidIndex = (size_t)fluidGridX * fluid->numCellsY + fluidGridY;

                    // add smoke at mouse click location
                    if (fluid->solidFlags[fluidIndex] == 1.0f) {
                        fluid->smokeDensity[fluidIndex] = fminf(fluid->smokeDensity[fluidIndex] + 0.8f, 1.0f);
                    }
                }
            } else if (e.type == SDL_MOUSEBUTTONUP) {
                if (e.button.button == SDL_BUTTON_LEFT) {
                    mouseIsDown = 0;
                }
            } else if (e.type == SDL_MOUSEMOTION) {
                if (mouseIsDown) {
                    int currentMouseX = e.motion.x;
                    int currentMouseY = e.motion.y;

                    float deltaX = (float)(currentMouseX - prevMouseX);
                    float deltaY = (float)(currentMouseY - prevMouseY); 

                    int fluidGridX = screen_to_fluid_x(currentMouseX, fluid->numCellsX, WINDOW_WIDTH);
                    int fluidGridY = screen_to_fluid_y(currentMouseY, fluid->numCellsY, WINDOW_HEIGHT);

                    fluidGridX = fmaxf(1, fminf(fluidGridX, fluid->numCellsX - 2));
                    fluidGridY = fmaxf(1, fminf(fluidGridY, fluid->numCellsY - 2));

                    size_t fluidIndex = (size_t)fluidGridX * fluid->numCellsY + fluidGridY;

                    // add smoke at mouse drag location
                    if (fluid->solidFlags[fluidIndex] == 1.0f) {
                        fluid->smokeDensity[fluidIndex] = fminf(fluid->smokeDensity[fluidIndex] + 0.8f, 1.0f);
                    }

                    float forceScale = 0.5f; 

                    // apply force to fluid based on mouse movement
                    if (fluid->solidFlags[fluidIndex] == 1.0f) {
                        fluid->velocityX[fluidIndex] += deltaX * forceScale;
                        fluid->velocityY[fluidIndex] += -deltaY * forceScale;
                    }
                    
                    prevMouseX = currentMouseX;
                    prevMouseY = currentMouseY;
                }
            }
        }

        // add smoke if mouse is held down
        if (mouseIsDown) {

            int fluidGridX = screen_to_fluid_x(prevMouseX, fluid->numCellsX, WINDOW_WIDTH);
            int fluidGridY = screen_to_fluid_y(prevMouseY, fluid->numCellsY, WINDOW_HEIGHT);

            fluidGridX = fmaxf(1, fminf(fluidGridX, fluid->numCellsX - 2));
            fluidGridY = fmaxf(1, fminf(fluidGridY, fluid->numCellsY - 2));

            size_t fluidIndex = (size_t)fluidGridX * fluid->numCellsY + fluidGridY;

            if (fluid->solidFlags[fluidIndex] == 1.0f) {
                fluid->smokeDensity[fluidIndex] = fminf(fluid->smokeDensity[fluidIndex] + 0.3f, 1.0f);
            }
        }

        // calculate delta time for simulation step
        Uint64 currentTick = SDL_GetPerformanceCounter();
        float deltaTime = (float)(currentTick - lastTick) / (float)SDL_GetPerformanceFrequency();
        lastTick = currentTick;

        // simulation parameters
        int numIterations = 100;
        float gravityForce = 0.0f;
        float overRelaxation = 1.9f;
        float velocityDissipation = 0.99f;
        float smokeDensityDissipation = 0.999f;

        fluid_simulate_step(fluid, numIterations, deltaTime, gravityForce, overRelaxation, velocityDissipation, smokeDensityDissipation);

        render_fluid(renderer, fluid);
    }

    // clean up resources
    fluid_free(fluid);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
