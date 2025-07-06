#include <SDL.h>
#include <stdio.h>
#include <math.h>
#include "ray_logic.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main(int argc, char* argv[]) {
    (void)argc;
    (void)argv;

    run_ray_tracer();

    return 0;
}

void run_ray_tracer(void) {
    SDL_Window* window = NULL; 
    SDL_Renderer* renderer = NULL; 
    int quit = 0; 
    SDL_Event event; 

    if (SDL_Init(SDL_INIT_VIDEO) < 0) { 
        fprintf(stderr, "ERROR: SDL could not initialize: %s\n", SDL_GetError()); 
        return; 
    }

    window = SDL_CreateWindow("Ray Tracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN); 
    if (window == NULL) { 
        fprintf(stderr, "ERROR: Window could not be created: %s\n", SDL_GetError()); 
        SDL_Quit(); 
        return; 
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED); 
    if (renderer == NULL) { 
        fprintf(stderr, "ERROR: Renderer could not be created: %s\n", SDL_GetError()); 
        SDL_DestroyWindow(window); 
        SDL_Quit(); 
        return; 
    }

    Sphere sphere1 = sphere_create((Vec3){0, 0, -5}, 1.0, (Color){255, 0, 0});
    Sphere sphere2 = sphere_create((Vec3){-2.5, 0, -5}, 1.0, (Color){0, 255, 0}); 
    Sphere floor = sphere_create((Vec3){0, -101, -5}, 100, (Color){100, 100, 100});

    Sphere scene[] = {sphere1, sphere2, floor};
    int num_spheres = sizeof(scene) / sizeof(Sphere);
    
    while (!quit) { 
        while (SDL_PollEvent(&event) != 0) { 
            if (event.type == SDL_QUIT) { 
                quit = 1; 
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderClear(renderer); 

        for (int y = 0; y < WINDOW_HEIGHT; ++y) {
            for (int x = 0; x < WINDOW_WIDTH; ++x) {
                double aspectRatio = (double)WINDOW_WIDTH / (double)WINDOW_HEIGHT;
                double ndc_x = (2.0 * (x + 0.5) / WINDOW_WIDTH - 1.0) * aspectRatio;
                double ndc_y = (1.0 - 2.0 * (y + 0.5) / WINDOW_HEIGHT);

                Ray ray;
                ray.origin = (Vec3){0, 0, 0}; 
                ray.direction = vec3_normalize((Vec3){ndc_x, ndc_y, -1.0});

                double closest_t = -1.0;
                Color pixel_color = {135, 206, 235}; 
                
                for(int i = 0; i < num_spheres; i++) {
                    double t;
                    if (ray_intersect_sphere(ray, scene[i], &t)) {
                        if (closest_t < 0 || t < closest_t) {
                           closest_t = t;
                           pixel_color = scene[i].color;
                        }
                    }
                }

                SDL_SetRenderDrawColor(renderer, pixel_color.x, pixel_color.y, pixel_color.z, 255);
                SDL_RenderDrawPoint(renderer, x, y);
            }
        }

        SDL_RenderPresent(renderer); 
    }

    SDL_DestroyRenderer(renderer); 
    SDL_DestroyWindow(window); 
    SDL_Quit(); 
}