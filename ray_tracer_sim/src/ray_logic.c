#include "ray_logic.h"

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