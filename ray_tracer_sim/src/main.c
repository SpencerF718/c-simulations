#include <SDL.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "ray_logic.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

int main(int argc, char* argv[]) {

    (void)argc;
    (void)argv;

    srand((unsigned int)time(NULL));

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
    double sphere1Reflectivity = 0.8;
    Sphere redSphere = sphere_create(sphere1Center, sphere1Radius, sphere1Color, sphere1Reflectivity);

    Vec3 sphere2Center = {1.0, -0.5, -3.0};
    double sphere2Radius = 0.8;
    Color sphere2Color = {0.0, 0.0, 1.0};
    double sphere2Reflectivity = 0.0;
    Sphere blueSphere = sphere_create(sphere2Center, sphere2Radius, sphere2Color, sphere2Reflectivity);

    Vec3 sphere3Center = {-2.0, 0.5, -7.0};
    double sphere3Radius = 1.2;
    Color sphere3Color = {0.0, 1.0, 0.0};
    double sphere3Reflectivity = 0.5;
    Sphere greenSphere = sphere_create(sphere3Center, sphere3Radius, sphere3Color, sphere3Reflectivity);

    Sphere sceneSpheres[] = {redSphere, blueSphere, greenSphere};
    int numSpheres = sizeof(sceneSpheres) / sizeof(sceneSpheres[0]);

    Vec3 lightPosition = {5.0, 5.0, 0.0};
    Color lightColor = {1.0, 1.0, 1.0};
    Color ambientLight = {0.1, 0.1, 0.1};

    Color specularLightColor = {1.0, 1.0, 1.0};
    double shininess = SHININESS_CONST;

    double lightRadius = 0.5;

    double aspectRatio = (double)WINDOW_WIDTH / WINDOW_HEIGHT;
    double halfFovRad = (cameraFov / 2.0) * (M_PI / 180);
    double halfHeight = tan(halfFovRad);
    double halfWidth =aspectRatio * halfHeight;

    int quitApplication = 0;
    SDL_Event eventHandler;

    Uint32 frameCount = 0;
    Uint32 lastFpsTime = SDL_GetTicks();
    char windowTitleBuffer[256];

    while (!quitApplication) {
        while (SDL_PollEvent(&eventHandler) != 0) {
            if (eventHandler.type == SDL_QUIT) {
                quitApplication = 1;
            } else if (eventHandler.type == SDL_MOUSEMOTION) {
                lightPosition.x = ((double)eventHandler.motion.x / WINDOW_WIDTH) * 10.0 - 5.0;
                lightPosition.y = ((double)eventHandler.motion.y / WINDOW_HEIGHT) * 10.0 - 5.0;
                lightPosition.z = 0.0;
            }
        }
        SDL_SetRenderDrawColor(sdlRenderer, 0x00, 0x00, 0x00, 0xFF);
        SDL_RenderClear(sdlRenderer);

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

                Color pixelColor = trace_ray(
                    primaryRay,
                    sceneSpheres,
                    numSpheres,
                    lightPosition,
                    lightColor,
                    ambientLight,
                    specularLightColor,
                    shininess,
                    lightRadius,
                    NUM_SHADOW_RAYS,
                    0
                );

                Uint8 r = (Uint8)(fmax(0.0, fmin(1.0, pixelColor.x)) * 255);
                Uint8 g = (Uint8)(fmax(0.0, fmin(1.0, pixelColor.y)) * 255);
                Uint8 b = (Uint8)(fmax(0.0, fmin(1.0, pixelColor.z)) * 255);

                SDL_SetRenderDrawColor(sdlRenderer, r, g, b, 0XFF);
                SDL_RenderDrawPoint(sdlRenderer, x, y);
            }
        }
        SDL_RenderPresent(sdlRenderer);

        frameCount++;
        Uint32 currentTime = SDL_GetTicks();

        if (currentTime - lastFpsTime >= ONE_SECOND) {
            double fps = frameCount / ((currentTime - lastFpsTime) / ONE_SECOND);
            snprintf(windowTitleBuffer, sizeof(windowTitleBuffer), "Ray Tracer Simulation - FPS: %.2f", fps);
            SDL_SetWindowTitle(sdlWindow, windowTitleBuffer);
            frameCount = 0;
            lastFpsTime = currentTime;
        }
    }

    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();

    return 0;
}


