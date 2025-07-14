#include <SDL.h>
#include <stdio.h>
#include <math.h>
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
    double cameraFov = DEFAULT_FOV;
    Camera sceneCamera = camera_create(cameraPosition,  cameraLookAt, cameraUpVector, cameraFov);

    Vec3 cameraForward = vec3_normalize(vec3_sub(sceneCamera.position, sceneCamera.lookAt));
    Vec3 cameraRight = vec3_normalize(vec3_cross(sceneCamera.upVector, cameraForward));
    Vec3 cameraUp = vec3_cross(cameraForward, cameraRight);

    Vec3 sphere1Center = {0.0, 0.0, -5.0};
    double sphere1Radius = 1.0;
    Color sphere1Color = {1.0, 0.0, 0.0};
    Sphere redSphere = sphere_create(sphere1Center, sphere1Radius, sphere1Color);

    Vec3 sphere2Center = {1.0, -0.5, -3.0};
    double sphere2Radius = 0.8;
    Color sphere2Color = {0.0, 0.0, 1.0};
    Sphere blueSphere = sphere_create(sphere2Center, sphere2Radius, sphere2Color);

    Vec3 sphere3Center = {-2.0, 0.5, -7.0};
    double sphere3Radius = 1.2;
    Color sphere3Color = {0.0, 1.0, 0.0};
    Sphere greenSphere = sphere_create(sphere3Center, sphere3Radius, sphere3Color);

    Sphere sceneSpheres[] = {redSphere, blueSphere, greenSphere};
    int numSpheres = sizeof(sceneSpheres) / sizeof(sceneSpheres[0]);

    double aspectRatio = (double)WINDOW_WIDTH / WINDOW_HEIGHT;
    double halfFovRad = (cameraFov / 2.0) * (M_PI / 180);
    double halfHeight = tan(halfFovRad);
    double halfWidth =aspectRatio * halfHeight;

    for (int y = 0; y < WINDOW_HEIGHT; y++) {
        for (int x = 0; x < WINDOW_WIDTH; x++) {

            double uNorm = (double)x / (WINDOW_WIDTH - 1.0) * 2.0 - 1.0;
            double vNorm = (double)y / (WINDOW_HEIGHT - 1.0) * 2.0 - 1.0;

            Vec3 rayDirection = {0};
            rayDirection = vec3_add(rayDirection, vec3_scale(cameraRight, uNorm * halfWidth));
            rayDirection = vec3_add(rayDirection, vec3_scale(cameraUp, vNorm * halfHeight));
            rayDirection = vec3_sub(rayDirection, cameraForward);

            rayDirection = vec3_normalize(rayDirection);
            Ray primaryRay = {sceneCamera.position, rayDirection};

            Color pixelColor = {0.0, 0.0, 0.0};
            double closestIntersectionDistance = INFINITY;
            Sphere* hitSphere = NULL;

            for (int i = 0; i < numSpheres; i++) {
                double currentIntersectionDistance;
                if (ray_intersect_sphere(primaryRay, sceneSpheres[i], &currentIntersectionDistance)) {
                    if (currentIntersectionDistance < closestIntersectionDistance) {
                        closestIntersectionDistance = currentIntersectionDistance;
                        hitSphere = &sceneSpheres[i];
                    }
                }
            }

            if (hitSphere != NULL) {
                pixelColor = hitSphere->color;
            }

            Uint8 r = (Uint8)(fmax(0.0, fmin(1.0, pixelColor.x)) * 255);
            Uint8 g = (Uint8)(fmax(0.0, fmin(1.0, pixelColor.y)) * 255);
            Uint8 b = (Uint8)(fmax(0.0, fmin(1.0, pixelColor.z)) * 255);

            SDL_SetRenderDrawColor(sdlRenderer, r, g, b, 0XFF);
            SDL_RenderDrawPoint(sdlRenderer, x, y);
        }
    }

    int quitApplication = 0;
    SDL_Event eventHandler;

    while (!quitApplication) {

        while (SDL_PollEvent(&eventHandler) != 0) {
            if (eventHandler.type == SDL_QUIT) {
                quitApplication = 1;
            }
        }
        SDL_RenderPresent(sdlRenderer);
    }

    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();

    return 0;
}


