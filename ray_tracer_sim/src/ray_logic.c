#include "ray_logic.h"
#include <math.h>

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
        return finalColor;
    }

    Vec3 intersectionPoint = vec3_add(ray.origin, vec3_scale(ray.direction, closestIntersectionDistance));
    Vec3 surfaceNormal = vec3_normalize(vec3_sub(intersectionPoint, hitSphere->center));
    
    if (vec3_dot(ray.direction, surfaceNormal) > 0) {
        surfaceNormal = vec3_scale(surfaceNormal, -1.0);
    }

    Vec3 lightDirection = vec3_normalize(vec3_sub(lightPosition, intersectionPoint));
    double diffuseFactor = fmax(0.0, vec3_dot(surfaceNormal, lightDirection));

    Vec3 viewDirection = vec3_normalize(vec3_scale(ray.direction, -1.0));
    Vec3 reflectionDirection = vec3_sub(vec3_scale(surfaceNormal, 2.0 * vec3_dot(surfaceNormal, lightDirection)), lightDirection);
    reflectionDirection = vec3_normalize(reflectionDirection);
    double specFactor = fmax(0.0, vec3_dot(reflectionDirection, viewDirection));
    specFactor = pow(specFactor, shininess);

    double shadowFactor = 0.0;
    int litSamples = 0;
    for (int i = 0; i < numShadowRays; ++i) {
        double u1 = (double)rand() / RAND_MAX * 2.0 - 1.0;
        double u2 = (double)rand() / RAND_MAX * 2.0 - 1.0;
        double u3 = (double)rand() / RAND_MAX * 2.0 - 1.0;
        Vec3 randomOffset = {u1, u2, u3};
        randomOffset = vec3_normalize(randomOffset);
        randomOffset = vec3_scale(randomOffset, lightRadius);

        Vec3 sampleLightPosition = vec3_add(lightPosition, randomOffset);

        Vec3 lightDirectionToSample = vec3_normalize(vec3_sub(sampleLightPosition, intersectionPoint));
        Ray shadowRay = {vec3_add(intersectionPoint, vec3_scale(surfaceNormal, EPSILON)), lightDirectionToSample};
        double lightDistanceToSample = vec3_length(vec3_sub(sampleLightPosition, intersectionPoint));

        int inShadow = 0;
        for (int k = 0; k < numSpheres; k++) {
            if (&spheres[k] != hitSphere) {
                double shadowIntersectionDistance;
                if (ray_intersect_sphere(shadowRay, spheres[k], &shadowIntersectionDistance)) {
                    if (shadowIntersectionDistance < lightDistanceToSample) {
                        inShadow = 1;
                        break;
                    }
                }
            }
        }
        if (!inShadow) {
            litSamples++;
        }
    }
    shadowFactor = (double)litSamples / numShadowRays;

    Color directLighting;
    directLighting.x = hitSphere->color.x * lightColor.x * diffuseFactor + specularLightColor.x * specFactor;
    directLighting.y = hitSphere->color.y * lightColor.y * diffuseFactor + specularLightColor.y * specFactor;
    directLighting.z = hitSphere->color.z * lightColor.z * diffuseFactor + specularLightColor.z * specFactor;

    directLighting.x *= shadowFactor;
    directLighting.y *= shadowFactor;
    directLighting.z *= shadowFactor;

    finalColor.x = ambientLight.x + directLighting.x;
    finalColor.y = ambientLight.y + directLighting.y;
    finalColor.z = ambientLight.z + directLighting.z;

    if (hitSphere->reflectivity > EPSILON && depth < MAX_RECURSION_DEPTH) {

        Vec3 incidentDirection = vec3_scale(ray.direction, -1.0);
        Vec3 reflectedDirection = vec3_sub(vec3_scale(surfaceNormal, 2.0 * vec3_dot(incidentDirection, surfaceNormal)), incidentDirection);
        reflectedDirection = vec3_normalize(reflectedDirection);

        Ray reflectedRay = {vec3_add(intersectionPoint, vec3_scale(surfaceNormal, EPSILON)), reflectedDirection};

        Color reflectedColor = trace_ray(
            reflectedRay,
            spheres,
            numSpheres,
            lightPosition,
            lightColor,
            ambientLight,
            specularLightColor,
            shininess,
            lightRadius,
            numShadowRays,
            depth + 1
        );
        
        finalColor.x = finalColor.x * (1.0 - hitSphere->reflectivity) + reflectedColor.x * hitSphere->reflectivity;
        finalColor.y = finalColor.y * (1.0 - hitSphere->reflectivity) + reflectedColor.y * hitSphere->reflectivity;
        finalColor.z = finalColor.z * (1.0 - hitSphere->reflectivity) + reflectedColor.z * hitSphere->reflectivity;
    }

    return finalColor;
}
