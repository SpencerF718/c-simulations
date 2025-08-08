#include "ray_logic.h"
#include <math.h>
#include <stdlib.h>

static inline unsigned int xorshift32(unsigned int *state) {
    unsigned int x = *state;
    if (x == 0) x = 0x1234567u;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static inline double xrnd_unit(unsigned int *state) {
    return (double)(xorshift32(state) & 0xFFFFFFu) / (double)0x1000000u;
}

Sphere sphere_create(Vec3 center, double radius, Color color, double reflectivity) { 
    Sphere s;
    s.center = center;
    s.radius = radius;
    s.color = color;
    s.reflectivity = reflectivity;
    return s;
}

Camera camera_create(Vec3 position, Vec3 lookAt, Vec3 upVector, double fov) {
    Camera c;
    c.position = position;
    c.lookAt = lookAt;
    c.upVector = upVector;
    c.fov = fov;
    return c;
}

int ray_intersect_sphere(Ray ray, Sphere sphere, double* intersectionDistance) {
    Vec3 oc = {ray.origin.x - sphere.center.x, ray.origin.y - sphere.center.y, ray.origin.z - sphere.center.z};
    double a = vec3_dot(ray.direction, ray.direction);
    double b = 2.0 * vec3_dot(oc, ray.direction);
    double c = vec3_dot(oc, oc) - sphere.radius * sphere.radius;
    double discriminant = b*b - 4*a*c;
    if (discriminant < 0) return 0;
    double sq = sqrt(discriminant);
    double t0 = (-b - sq) / (2*a);
    double t1 = (-b + sq) / (2*a);
    double t = t0;
    if (t < EPSILON) t = t1;
    if (t < EPSILON) return 0;
    *intersectionDistance = t;
    return 1;
}

Vec3 vec3_add(Vec3 a, Vec3 b) {
    Vec3 result = {a.x + b.x, a.y + b.y, a.z + b.z};
    return result;
}
Vec3 vec3_sub(Vec3 a, Vec3 b) {
    Vec3 result = {a.x - b.x, a.y - b.y, a.z - b.z};
    return result;
}
Vec3 vec3_scale(Vec3 v, double s) {
    Vec3 result = {v.x * s, v.y * s, v.z * s};
    return result;
}
double vec3_dot(Vec3 a, Vec3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}
Vec3 vec3_cross(Vec3 a, Vec3 b) {
    Vec3 result = {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return result;
}
double vec3_length(Vec3 v) {
    return sqrt(vec3_dot(v, v));
}
Vec3 vec3_normalize(Vec3 v) {
    double len = vec3_length(v);
    if (len < 1e-12) return v;
    return vec3_scale(v, 1.0 / len);
}

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
    int depth,
    unsigned int *rngState
) {
    Color finalColor = {0.0, 0.0, 0.0};
    double closestIntersectionDistance = INFINITY;
    Sphere* hitSphere = NULL;

    for (int i = 0; i < numSpheres; i++) {
        double currentIntersectionDistance;
        if (ray_intersect_sphere(ray, spheres[i], &currentIntersectionDistance)) {
            if (currentIntersectionDistance < closestIntersectionDistance) {
                closestIntersectionDistance = currentIntersectionDistance;
                hitSphere = &spheres[i];
            }
        }
    }

    if (hitSphere == NULL) {
        Color black = {0.0, 0.0, 0.0};
        return black;
    }

    Vec3 hitPoint = vec3_add(ray.origin, vec3_scale(ray.direction, closestIntersectionDistance));
    Vec3 normal = vec3_sub(hitPoint, hitSphere->center);
    normal = vec3_normalize(normal);
    Vec3 viewDir = vec3_scale(ray.direction, -1.0);
    viewDir = vec3_normalize(viewDir);

    finalColor.x = ambientLight.x * hitSphere->color.x;
    finalColor.y = ambientLight.y * hitSphere->color.y;
    finalColor.z = ambientLight.z * hitSphere->color.z;

    int hits = 0;
    for (int i = 0; i < numShadowRays; ++i) {
        double u1 = xrnd_unit(rngState) * 2.0 - 1.0;
        double u2 = xrnd_unit(rngState) * 2.0 - 1.0;
        double u3 = xrnd_unit(rngState) * 2.0 - 1.0;
        Vec3 randomOffset = {u1, u2, u3};
        randomOffset = vec3_scale(vec3_normalize(randomOffset), lightRadius);

        Vec3 lightSample = vec3_add(lightPosition, randomOffset);
        Vec3 toLight = vec3_sub(lightSample, hitPoint);
        double distToLight = vec3_length(toLight);
        toLight = vec3_normalize(toLight);

        Ray shadowRay = { vec3_add(hitPoint, vec3_scale(normal, EPSILON)), toLight };
        int blocked = 0;
        for (int j = 0; j < numSpheres; ++j) {
            double ttmp;
            if (ray_intersect_sphere(shadowRay, spheres[j], &ttmp)) {
                if (ttmp < distToLight) {
                    blocked = 1;
                    break;
                }
            }
        }
        if (!blocked) hits++;
    }

    double visibility = (double)hits / (double)numShadowRays;

    Vec3 lightDir = vec3_normalize(vec3_sub(lightPosition, hitPoint));
    double diff = vec3_dot(normal, lightDir);
    if (diff > 0.0) {
        finalColor.x += diff * hitSphere->color.x * lightColor.x * visibility;
        finalColor.y += diff * hitSphere->color.y * lightColor.y * visibility;
        finalColor.z += diff * hitSphere->color.z * lightColor.z * visibility;
    }

    Vec3 reflectDir = vec3_sub(vec3_scale(normal, 2.0 * vec3_dot(normal, lightDir)), lightDir);
    reflectDir = vec3_normalize(reflectDir);
    double spec = pow(fmax(0.0, vec3_dot(reflectDir, viewDir)), shininess);
    finalColor.x += spec * specularLightColor.x * visibility;
    finalColor.y += spec * specularLightColor.y * visibility;
    finalColor.z += spec * specularLightColor.z * visibility;

    if (depth < MAX_RECURSION_DEPTH && hitSphere->reflectivity > 0.0) {
        Vec3 reflDir = vec3_sub(ray.direction, vec3_scale(normal, 2.0 * vec3_dot(ray.direction, normal)));
        reflDir = vec3_normalize(reflDir);
        Ray reflRay = { vec3_add(hitPoint, vec3_scale(normal, EPSILON)), reflDir };
        Color reflectedColor = trace_ray(
            reflRay,
            spheres,
            numSpheres,
            lightPosition,
            lightColor,
            ambientLight,
            specularLightColor,
            shininess,
            lightRadius,
            numShadowRays,
            depth + 1,
            rngState
        );

        finalColor.x = finalColor.x * (1.0 - hitSphere->reflectivity) + reflectedColor.x * hitSphere->reflectivity;
        finalColor.y = finalColor.y * (1.0 - hitSphere->reflectivity) + reflectedColor.y * hitSphere->reflectivity;
        finalColor.z = finalColor.z * (1.0 - hitSphere->reflectivity) + reflectedColor.z * hitSphere->reflectivity;
    }

    return finalColor;
}

Color trace_ray_with_rng(
    Ray ray,
    Sphere *spheres,
    int numSpheres,
    Vec3 lightPos,
    int shadow_samples,
    unsigned int *rngState
) {
    Color lightColor = {1.0, 1.0, 1.0};
    Color ambientLight = {0.1, 0.1, 0.1};
    Color specularLightColor = {1.0, 1.0, 1.0};
    double shininess = SHININESS_CONST;
    double lightRadius = 0.5;

    return trace_ray(
        ray,
        spheres,
        numSpheres,
        lightPos,
        lightColor,
        ambientLight,
        specularLightColor,
        shininess,
        lightRadius,
        shadow_samples,
        0,
        rngState
    );
}
