#include <SDL.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdatomic.h>
#include <math.h>
#include "ray_logic.h"

static inline unsigned int xorshift32(unsigned int *state) {
    unsigned int x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x ? x : 0x1234567u;
    return *state;
}
static inline double xrnd_unit(unsigned int *state) {
    return (double)(xorshift32(state) & 0xFFFFFFu) / (double)0x1000000u;
}

typedef struct {
    Camera camera;
    Sphere *spheres;
    int numSpheres;
    Vec3 *precompRays;
    int width, height;
    Vec3 lightPos;
    int shadow_samples;
    uint32_t *pixels;
    atomic_int next_row;
} RenderJob;

extern Color trace_ray_with_rng(Ray r,
                                Sphere *spheres, int numSpheres,
                                Vec3 lightPos,
                                int shadow_samples,
                                unsigned int *rng_state);

static int sdl_render_thread(void *arg) {
    RenderJob *job = (RenderJob *)arg;

    unsigned int seed = (unsigned int)time(NULL);
    seed ^= (unsigned int)(uintptr_t)job;
    seed ^= (unsigned int)SDL_GetTicks();
    if (seed == 0) seed = 123456789u;

    while (1) {
        int y = atomic_fetch_add(&job->next_row, 1);
        if (y >= job->height) break;

        uint32_t *rowptr = job->pixels + (size_t)y * job->width;

        for (int x = 0; x < job->width; ++x) {
            Vec3 dir = job->precompRays[y * job->width + x];
            Ray primary = { job->camera.position, dir };

            Color col = trace_ray_with_rng(primary,
                                           job->spheres, job->numSpheres,
                                           job->lightPos,
                                           job->shadow_samples,
                                           &seed);

            float rx = fmaxf(0.0f, fminf(1.0f, (float)col.x));
            float gx = fmaxf(0.0f, fminf(1.0f, (float)col.y));
            float bx = fmaxf(0.0f, fminf(1.0f, (float)col.z));

            uint8_t R = (uint8_t)(rx * 255.0f);
            uint8_t G = (uint8_t)(gx * 255.0f);
            uint8_t B = (uint8_t)(bx * 255.0f);

            rowptr[x] = ((uint32_t)R << 16) | ((uint32_t)G << 8) | (uint32_t)B;
        }
    }
    return 0;
}

void render_frame_sdl_threads(RenderJob *job) {
    if (!job || !job->pixels || !job->precompRays) return;

    int nthreads = SDL_GetCPUCount();
    if (nthreads < 1) nthreads = 1;

    SDL_Thread **threads = (SDL_Thread**)malloc(sizeof(SDL_Thread*) * nthreads);
    if (!threads) {
        fprintf(stderr, "Out of memory creating thread array\n");
        return;
    }

    atomic_store(&job->next_row, 0);

    for (int i = 0; i < nthreads; ++i) {
        char namebuf[32];
        snprintf(namebuf, sizeof(namebuf), "rt_worker_%d", i);
        threads[i] = SDL_CreateThread(sdl_render_thread, namebuf, (void*)job);
        if (!threads[i]) {
            fprintf(stderr, "SDL_CreateThread failed: %s\n", SDL_GetError());
            nthreads = i;
            break;
        }
    }

    for (int i = 0; i < nthreads; ++i) {
        int threadReturnValue;
        SDL_WaitThread(threads[i], &threadReturnValue);
        (void)threadReturnValue;
    }

    free(threads);
}
