#ifndef RAY_LOGIC_H
#define RAY_LOGIC_H

#include <math.h>
#include <stdio.h>
#include <SDL.h>


#define EPSILON 0.001
#define DEFAULT_FOV 90
#define ONE_SECOND 1000.0f
#define SHININESS_CONST 64.0
#define NUM_SHADOW_RAYS 32
#define MAX_RECURSION_DEPTH 3

// 3D vector struct
typedef struct {
    double x;
    double y;
    double z;
} Vec3;

// color struct with xyz mapped to RGB
typedef Vec3 Color;

// ray struct
typedef struct {
    Vec3 origin;
    Vec3 direction;
} Ray;

// sphere struct
typedef struct {
    Vec3 center;
    double radius;
    Color color;
    double reflectivity;
} Sphere;

typedef struct {
    Vec3 position;
    Vec3 lookAt;
    Vec3 upVector;
    double fov;
} Camera;

// vec3 math functions
Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_sub(Vec3 a, Vec3 b);
Vec3 vec3_scale(Vec3 v, double s);
double vec3_dot(Vec3 a, Vec3 b);
Vec3 vec3_cross(Vec3 a, Vec3 b);
double vec3_length(Vec3 v);
Vec3 vec3_normalize(Vec3 v);

// function to create a new sphere
Sphere sphere_create(Vec3 center, double radius, Color color, double reflectivity);

// function to create a new camera
Camera camera_create(Vec3 position, Vec3 lookAt, Vec3 upVector, double fov);

// ray intercetion (integer representation of a boolean)
int ray_intersect_sphere(Ray ray, Sphere sphere, double* intersectionDistance);

// function containing the main ray tracing logic for a single ray
Color trace_ray(
    Ray ray,
    Sphere* spheres,
    int numSpheres,
    Vec3 lightPosition,
    Color lightColor,
    Color ambientLight,
    Color specularLightColor,
    double shininess,
    double lightRadius,
    int numShadowRays,
    int depth
);

// main ray tracing loop
void run_ray_tracer(void);

#endif
