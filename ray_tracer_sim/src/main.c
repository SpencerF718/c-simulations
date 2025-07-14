#include <SDL.h>
#include <stdio.h>
#include <math.h>
#include "ray_logic.h"

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 300

void update_camera(Camera* camera, double delta_time, double base_move_speed, int forward, int backward, int left, int right) {
    Vec3 forward_vec = vec3_normalize(vec3_sub(camera->lookAt, camera->position));
    Vec3 right_vec = vec3_normalize(vec3_cross(forward_vec, camera->upVector));

    double current_move_speed = base_move_speed * delta_time; 

    if (forward) {
        camera->position = vec3_add(camera->position, vec3_scale(forward_vec, current_move_speed));
        camera->lookAt = vec3_add(camera->lookAt, vec3_scale(forward_vec, current_move_speed));
    }
    if (backward) {
        camera->position = vec3_sub(camera->position, vec3_scale(forward_vec, current_move_speed));
        camera->lookAt = vec3_sub(camera->lookAt, vec3_scale(forward_vec, current_move_speed));
    }
    if (left) {
        camera->position = vec3_sub(camera->position, vec3_scale(right_vec, current_move_speed));
        camera->lookAt = vec3_sub(camera->lookAt, vec3_scale(right_vec, current_move_speed));
    }
    if (right) {
        camera->position = vec3_add(camera->position, vec3_scale(right_vec, current_move_speed));
        camera->lookAt = vec3_add(camera->lookAt, vec3_scale(right_vec, current_move_speed));
    }
}

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

    int move_forward = 0;
    int move_backward = 0;
    int move_left = 0;
    int move_right = 0;
    double base_camera_move_speed = 5.0;

    Uint32 last_tick = SDL_GetTicks();
    Uint32 current_tick;
    double delta_time;

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

    Camera main_camera = camera_create((Vec3){0, 0, 0}, (Vec3){0, 0, -1}, (Vec3){0, 1, 0}, 60.0);

    Sphere sphere1 = sphere_create((Vec3){0, 0, -5}, 1.0, (Color){255, 0, 0});
    Sphere sphere2 = sphere_create((Vec3){-2.5, 0, -5}, 1.0, (Color){0, 255, 0});
    Sphere floor = sphere_create((Vec3){0, -101, -5}, 100, (Color){100, 100, 100});

    Sphere scene[] = {sphere1, sphere2, floor};
    int num_spheres = sizeof(scene) / sizeof(Sphere);

    while (!quit) {
        current_tick = SDL_GetTicks();
        delta_time = (double)(current_tick - last_tick) / 1000.0;
        last_tick = current_tick;

        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = 1;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_w: move_forward = 1; break;
                    case SDLK_s: move_backward = 1; break;
                    case SDLK_a: move_left = 1; break;
                    case SDLK_d: move_right = 1; break;
                }
            } else if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_w: move_forward = 0; break;
                    case SDLK_s: move_backward = 0; break;
                    case SDLK_a: move_left = 0; break;
                    case SDLK_d: move_right = 0; break;
                }
            }
        }

        update_camera(&main_camera, delta_time, base_camera_move_speed, move_forward, move_backward, move_left, move_right);


        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (int y = 0; y < WINDOW_HEIGHT; ++y) {
            for (int x = 0; x < WINDOW_WIDTH; ++x) {
                double aspectRatio = (double)WINDOW_WIDTH / (double)WINDOW_HEIGHT;
                double ndc_x = (2.0 * (x + 0.5) / WINDOW_WIDTH - 1.0) * aspectRatio;
                double ndc_y = (1.0 - 2.0 * (y + 0.5) / WINDOW_HEIGHT);

                Vec3 camera_forward = vec3_normalize(vec3_sub(main_camera.lookAt, main_camera.position));
                Vec3 camera_right = vec3_normalize(vec3_cross(camera_forward, main_camera.upVector));
                Vec3 camera_up = vec3_normalize(vec3_cross(camera_right, camera_forward));

                double fov_rad = main_camera.fov * M_PI / 180.0;
                double half_fov_y = tan(fov_rad / 2.0);
                double half_fov_x = half_fov_y * aspectRatio;

                Vec3 ray_direction = vec3_add(
                    vec3_add(vec3_scale(camera_right, ndc_x * half_fov_x),
                             vec3_scale(camera_up, ndc_y * half_fov_y)),
                    camera_forward
                );
                ray_direction = vec3_normalize(ray_direction);

                Ray ray;
                ray.origin = main_camera.position;
                ray.direction = ray_direction;

                double closest_t = -1.0;
                Color pixel_color = {135, 206, 235};

                for(int i = 0; i < num_spheres; i++) {
                    double t;
                    if (ray_intersect_sphere(ray, scene[i], &t)) {
                        if (closest_t < 0 || (t > 0.001 && t < closest_t)) {
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
