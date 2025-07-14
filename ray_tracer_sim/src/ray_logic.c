#include "ray_logic.h"
#include <math.h>

Sphere sphere_create(Vec3 center, double radius, Color color) { 
    Sphere s;
    s.center = center;
    s.radius = radius;
    s.color = color;
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

Vec3 vec3_add(Vec3 a, Vec3 b) {
    Vec3 result;
    result.x = a.x + b.x;
    result.y = a.y + b.y;
    result.z = a.z + b.z;
    return result;
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
    Vec3 result;
    result.x = a.x - b.x;
    result.y = a.y - b.y;
    result.z = a.z - b.z;
    return result;
}

Vec3 vec3_scale(Vec3 v, double s) {
    Vec3 result;
    result.x = v.x * s;
    result.y = v.y * s;
    result.z = v.z * s;
    return result;
}

double vec3_dot(Vec3 a, Vec3 b) {
    double result = a.x * b.x + a.y * b.y + a.z * b.z;
    return result;
}

Vec3 vec3_cross(Vec3 a, Vec3 b) {
    Vec3 result;
    result.x = a.y * b.z - a.z * b.y;
    result.y = a.z * b.x - a.x * b.z;
    result.z = a.x * b.y - a.y * b.x;
    return result;
}

double vec3_length(Vec3 v) {
    double result = sqrt(vec3_dot(v, v));
    return result;
}

Vec3 vec3_normalize(Vec3 v) {
    Vec3 result = {0, 0, 0};
    double len = vec3_length(v);
    if (len > 0) {
        result.x = v.x / len;
        result.y = v.y / len;
        result.z = v.z / len;
    }
    return result;
}

int ray_intersect_sphere(Ray ray, Sphere sphere, double* intersectionDistance) {
    Vec3 oc = vec3_sub(ray.origin, sphere.center);

    double a = vec3_dot(ray.direction, ray.direction);
    double b = 2.0 * vec3_dot(oc, ray.direction);
    double c = vec3_dot(oc, oc) - sphere.radius * sphere.radius;

    double discriminant = b * b - 4 * a * c;

    if (discriminant < 0) {
        return 0; 
    } else {
        double closestDistance = (-b - sqrt(discriminant)) / (2.0 * a);
        double fartherDistance = (-b + sqrt(discriminant)) / (2.0 * a);

        if (closestDistance > EPSILON) {
            *intersectionDistance = closestDistance;
            return 1;
        }
        if (fartherDistance > EPSILON) {
            *intersectionDistance = fartherDistance;
            return 1;
        }
        return 0;
    }
}
