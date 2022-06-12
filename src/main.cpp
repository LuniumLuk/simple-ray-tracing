#include <stdio.h>
#include <stdlib.h>
#include "global.hpp"
#include "image.hpp"
#include "mesh.hpp"
#include "geometry.hpp"

#define COLOR_PRINT(vec) printf("Vector4: (%f, %f, %f, %f)\n", vec.r, vec.g, vec.b, vec.a)
#define WRITE_COLOR(image,x,y,color) do { image.pixel_at(x, y, 0) = color.r;  \
                                          image.pixel_at(x, y, 1) = color.g;  \
                                          image.pixel_at(x, y, 2) = color.b; } while(0)

Geometry::Sphere sp(vec3(0.0f, 0.0f, -1.0f), 0.5f);

vec4 ray_color(const Geometry::Ray & r) {
    Geometry::HitRecord rec;
    if (sp.hit(r, 0.0f, 100.0f, rec))
    {
        return vec4(0.5f * (rec.normal + 1.0f), 1.0);
    }
    vec3 unit_direction = glm::normalize(r.direction());
    unit_direction = 0.5f * (unit_direction + 1.0f);
    return vec4(0.5f * (unit_direction + 1.0f), 1.0f);
}

int main()
{
    // Image

    const float aspect_ratio = 16.0f / 9.0f;
    const int scr_h = 900;
    const int scr_w = static_cast<int>(scr_h * aspect_ratio);
    Utility::Image image(scr_w, scr_h, 3);

    // Camera

    float viewport_h = 2.0f;
    float viewport_w = aspect_ratio * viewport_h;
    float focal_length = 1.0f;

    vec3 origin = vec3(0.0f, 0.0f, 0.0f);
    vec3 horizontal = vec3(viewport_w, 0.0f, 0.0f);
    vec3 vertical = vec3(0.0f, viewport_h, 0.0f);
    vec3 lower_left_corner = origin - horizontal * 0.5f - vertical * 0.5f - vec3(0.0f, 0.0f, focal_length);

    // Render

    for (int j = image.height() - 1; j >= 0; j--) {
        printf("\r[INFO] Scanlines remaining: %-5d", j);
        for (int i = 0; i < image.width(); ++i) {
            float u = float(i) / (image.width() - 1);
            float v = float(j) / (image.height() - 1);
            Geometry::Ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
            vec4 pixel_color = ray_color(r);
            WRITE_COLOR(image, i, j, pixel_color);
        }
    }

    printf("\n[INFO] Done!\n");

    image.save("result.png");

    return 0;
} 