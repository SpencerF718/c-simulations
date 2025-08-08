#include <SDL.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdatomic.h>
#include "ray_logic.h"

#ifdef _MSC_VER
#define snprintf _snprintf
#endif


const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;

typedef struct {
    Camera camera;
    Sphere *spheres;
    int numSpheres;
    Vec3 *precompRays;
    int width;
    int height;
    Vec3 lightPos;
    int shadowSamples;
    uint32_t *pixels;
    atomic_int nextRow;
} RenderJob;

static int sdlRenderWorker(void *arg) {
    RenderJob *job = (RenderJob *)arg;
    unsigned int seed = (unsigned int)time(NULL);
    seed ^= (unsigned int)(uintptr_t)job;
    seed ^= (unsigned int)SDL_GetTicks();
    if (seed == 0) seed = 0x1234567u;

    while (1) {
        int y = atomic_fetch_add(&job->nextRow, 1);
        if (y >= job->height) break;
        uint32_t *rowPtr = job->pixels + (size_t)y * job->width;
        for (int x = 0; x < job->width; ++x) {
            Vec3 dir = job->precompRays[y * job->width + x];
            Ray primary = { job->camera.position, dir };
            Color col = trace_ray(
                primary,
                job->spheres,
                job->numSpheres,
                job->lightPos,
                (Color){1.0f, 1.0f, 1.0f},
                (Color){0.1f, 0.1f, 0.1f},
                (Color){1.0f, 1.0f, 1.0f},
                SHININESS_CONST,
                0.5f,
                job->shadowSamples,
                0,
                &seed
            );
            float rx = fmaxf(0.0f, fminf(1.0f, (float)col.x));
            float gx = fmaxf(0.0f, fminf(1.0f, (float)col.y));
            float bx = fmaxf(0.0f, fminf(1.0f, (float)col.z));
            uint8_t R = (uint8_t)(rx * 255.0f);
            uint8_t G = (uint8_t)(gx * 255.0f);
            uint8_t B = (uint8_t)(bx * 255.0f);
            rowPtr[x] = ((uint32_t)R << 16) | ((uint32_t)G << 8) | (uint32_t)B;
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Ray Tracer (SDL threads)",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) { SDL_Quit(); return 1; }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) { SDL_DestroyWindow(window); SDL_Quit(); return 1; }

    SDL_Texture* texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_RGB888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!texture) { SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit(); return 1; }

    uint32_t *pixels = (uint32_t*)malloc((size_t)WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(uint32_t));
    if (!pixels) { SDL_DestroyTexture(texture); SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); SDL_Quit(); return 1; }

    Camera sceneCamera = camera_create((Vec3){0.0f, 0.0f, 0.0f}, (Vec3){0.0f, 0.0f, -1.0f}, (Vec3){0.0f, 1.0f, 0.0f}, DEFAULT_FOV);

    Vec3 cameraForward = vec3_normalize(vec3_sub(sceneCamera.position, sceneCamera.lookAt));
    Vec3 cameraRight = vec3_normalize(vec3_cross(sceneCamera.upVector, cameraForward));
    Vec3 cameraUp = vec3_cross(cameraForward, cameraRight);

    Sphere redSphere = sphere_create((Vec3){0.0f, 0.0f, -5.0f}, 1.0f, (Color){1.0f, 0.0f, 0.0f}, 0.8f);
    Sphere blueSphere = sphere_create((Vec3){1.0f, -0.5f, -3.0f}, 0.8f, (Color){0.0f, 0.0f, 1.0f}, 0.0f);
    Sphere greenSphere = sphere_create((Vec3){-2.0f, 0.5f, -7.0f}, 1.2f, (Color){0.0f, 1.0f, 0.0f}, 0.5f);

    Sphere sceneSpheres[] = {redSphere, blueSphere, greenSphere};
    int numSpheres = sizeof(sceneSpheres) / sizeof(sceneSpheres[0]);

    Vec3 lightPosition = {5.0f, 5.0f, 0.0f};

    Vec3 *precompRays = (Vec3*)malloc((size_t)WINDOW_WIDTH * WINDOW_HEIGHT * sizeof(Vec3));
    float aspectRatio = (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT;
    float halfFovRad = (sceneCamera.fov / 2.0f) * (3.14159265358979323846f / 180.0f);
    float halfHeight = tanf(halfFovRad);
    float halfWidth = aspectRatio * halfHeight;

    for (int y = 0; y < WINDOW_HEIGHT; ++y) {
        for (int x = 0; x < WINDOW_WIDTH; ++x) {
            float uNorm = (float)x / ((float)WINDOW_WIDTH - 1.0f) * 2.0f - 1.0f;
            float vNorm = (float)y / ((float)WINDOW_HEIGHT - 1.0f) * 2.0f - 1.0f;
            Vec3 rdir = {0};
            rdir = vec3_add(rdir, vec3_scale(cameraRight, uNorm * halfWidth));
            rdir = vec3_add(rdir, vec3_scale(cameraUp, vNorm * halfHeight));
            rdir = vec3_sub(rdir, cameraForward);
            rdir = vec3_normalize(rdir);
            precompRays[y * WINDOW_WIDTH + x] = rdir;
        }
    }

    RenderJob job;
    job.camera = sceneCamera;
    job.spheres = sceneSpheres;
    job.numSpheres = numSpheres;
    job.precompRays = precompRays;
    job.width = WINDOW_WIDTH;
    job.height = WINDOW_HEIGHT;
    job.lightPos = lightPosition;
    job.shadowSamples = 8;
    job.pixels = pixels;
    atomic_init(&job.nextRow, 0);

    int quit = 0;
    SDL_Event ev;
    Uint32 frameCount = 0;
    Uint32 lastFps = SDL_GetTicks();
    char title[128];

    while (!quit) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) quit = 1;
            else if (ev.type == SDL_MOUSEMOTION) {
                job.lightPos.x = ((float)ev.motion.x / (float)WINDOW_WIDTH) * 10.0f - 5.0f;
                job.lightPos.y = ((float)ev.motion.y / (float)WINDOW_HEIGHT) * 10.0f - 5.0f;
                job.lightPos.z = 0.0f;
            }
        }

        int nthreads = SDL_GetCPUCount();
        if (nthreads < 1) nthreads = 1;

        SDL_Thread **threads = (SDL_Thread**)malloc(sizeof(SDL_Thread*) * nthreads);
        if (!threads) break;

        atomic_store(&job.nextRow, 0);
        for (int i = 0; i < nthreads; ++i) {
            char nameBuf[32];
            snprintf(nameBuf, sizeof(nameBuf), "rtWorker_%d", i);
            threads[i] = SDL_CreateThread(sdlRenderWorker, nameBuf, &job);
            if (!threads[i]) {
                nthreads = i;
                break;
            }
        }

        for (int i = 0; i < nthreads; ++i) {
            int ret;
            SDL_WaitThread(threads[i], &ret);
            (void)ret;
        }

        free(threads);

        SDL_UpdateTexture(texture, NULL, pixels, WINDOW_WIDTH * sizeof(uint32_t));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        frameCount++;
        Uint32 now = SDL_GetTicks();
        if (now - lastFps >= ONE_SECOND) {
            double fps = frameCount / ((now - lastFps) / (double)ONE_SECOND);
            snprintf(title, sizeof(title), "Ray Tracer (SDL threads) - FPS: %.2f", fps);
            SDL_SetWindowTitle(window, title);
            frameCount = 0;
            lastFps = now;
        }
    }

    free(precompRays);
    free(pixels);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}


