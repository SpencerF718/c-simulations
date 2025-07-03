#include "ray_logic.h"

Sphere sphere_create(Vec3 center, double radius, Color color) {
    
    Sphere s;
    s.center = center;
    s.radius = radius;
    s.color = color;

    return s;

}