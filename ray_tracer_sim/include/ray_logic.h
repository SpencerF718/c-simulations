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

// Sphere struct
typedef struct {
    Vec3 center;
    double radius
    Color color;
}

// color struct with xyz mapped to RGB
typedef Vec3 Color;

// function to create a new sphere
Sphere sphere_create(Vec3 center, double radius, Color color);

// main ray tracing loop
void run_ray_tracer(void);

#endif
