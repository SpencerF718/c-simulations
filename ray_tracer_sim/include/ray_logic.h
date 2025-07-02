#ifndef RAY_LOGIC_H
#define RAY_LOGIC_H

#include <SDL.h>

// 3D vector struct
typedef struct {
    double x;
    double y;
    double z;
} Vec3;

// Ray struct
typedef struct {
    Vec3 origin;
    Vec3 direction;
} Ray;

// color struct with xyz mapped to RGB
typedef Vec3 Color;

// main ray tracing loop
void run_ray_tracer(void);

#endif
