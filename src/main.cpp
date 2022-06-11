#include <stdio.h>
#include <stdlib.h>
#include "global.hpp"
#include "image.hpp"
#include "mesh.hpp"

#define COLOR_PRINT(vec) printf("Vector4: (%f, %f, %f, %f)\n", vec.r, vec.g, vec.b, vec.a)

int main()
{
    // Image

    const float aspect_ratio = 16.0f / 9.0f;
    const int scr_h = 450;
    const int scr_w = static_cast<int>(scr_h * aspect_ratio);
    Utility::Image frame_buffer(scr_w, scr_h, 3);

    // Camera

    float viewport_h = 2.0f;
    float viewport_w = aspect_ratio * viewport_h;
    float focal_length = 1.0f;

    vec3 origin = vec3(0.0f, 0.0f, 0.0f);
    vec3 horizontal = vec3(viewport_w, 0.0f, 0.0f);
    vec3 vertical = vec3(0.0f, viewport_h, 0.0f);
    vec3 lower_left_corner = origin - horizontal * 0.5f - vertical * 0.5f - vec3(0.0f, 0.0f, focal_length);

    return 0;
} 