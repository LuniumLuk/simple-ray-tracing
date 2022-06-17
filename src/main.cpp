#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <time.h>
#include "global.hpp"
#include "image.hpp"
#include "mesh.hpp"
#include "geometry.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "scene.hpp"
#ifdef _OPENMP
#include <omp.h>
#endif

#define WRITE_COLOR(image,x,y,color) do { image.pixel_at(x, y, 0) = color.r;  \
                                          image.pixel_at(x, y, 1) = color.g;  \
                                          image.pixel_at(x, y, 2) = color.b; } while(0)

vec4 ray_color(const Geometry::Ray & r, const Geometry::Hittable & world, int depth);

int main()
{
    // Image

    const float aspect_ratio = 3.0f / 2.0f;
    const int scr_h = 400;
    const int scr_w = static_cast<int>(scr_h * aspect_ratio);
    const int samples_per_pixel = 64;
    const int max_depth = 32;

    Utility::Image image(scr_w, scr_h, 3);

    // World

    Geometry::HittableList world = generate_simple_scene();
    // Geometry::HittableList world = generate_random_scene();

    // Camera

    // vec3 eye = vec3(13.0f, 2.0f, 3.0f);
    vec3 eye = vec3( 0.0f, 4.0f, 6.0f);
    vec3 at  = vec3( 0.0f, 0.0f, 0.0f);
    vec3 up  = vec3( 0.0f, 1.0f, 0.0f);
    float aperture = 0.1f;
    float focal_length = 10.0f;

    Scene::Camera camera(eye, at, up, 90.0f, aspect_ratio, aperture, focal_length, 1.0f);

    // Render
    clock_t last_timestamp = clock();
    clock_t start_timestamp = last_timestamp;
    float duration;
    char estimate_time[DURATION_STR_LENGTH];
#ifdef _OPENMP
    printf("[INFO] Omp max threads: %d\n", omp_get_max_threads());
#endif
    for (int j = image.height() - 1; j >= 0; j--)
    {
#ifdef _OPENMP
#pragma omp parallel for
#endif
        for (int i = 0; i < image.width(); ++i)
        {
            vec4 pixel_color(0.0f, 0.0f, 0.0f, 1.0f);
            for (int s = 0; s < samples_per_pixel; s++)
            {
                float u = ((float)i + random_float()) / (image.width() - 1);
                float v = ((float)j + random_float()) / (image.height() - 1);
                Geometry::Ray r = camera.get_ray(u, v);
                pixel_color += ray_color(r, world, max_depth);
            }
            pixel_color *= 1.0f / samples_per_pixel;
            WRITE_COLOR(image, i, j, pixel_color);
        }
        duration = ((float)(clock() - last_timestamp) / CLOCKS_PER_SEC);
        get_duration_str(duration * j, estimate_time);
        printf("\r[INFO] Scanlines remaining: % 4d, % 5.2f scanlines per second, Estimated time left: %s", j, 1.0 / duration, estimate_time);
        fflush(stdout);
        last_timestamp = clock();
    }
    printf("\n");
    
    char total_time[DURATION_STR_LENGTH];
    duration = ((float)(clock() - start_timestamp) / CLOCKS_PER_SEC);
    get_duration_str(duration, total_time);
    printf("[INFO] Done! Total time: %s\n", total_time);

    image.save("result.png");

    return 0;
}

vec4 ray_color(const Geometry::Ray & r, const Geometry::Hittable & world, int depth)
{
    if (depth <= 0) return COLOR_BLACK;

    Geometry::HitRecord rec;
    if (world.hit(r, 0.001f, FLOAT_INFINITY, rec))
    {
        Geometry::Ray scattered;
        vec4 attenuation;
        vec4 emissive = rec.material->emitted();
        if (rec.material->scatter(r, rec, attenuation, scattered))
        {
            return emissive + attenuation * ray_color(scattered, world, depth - 1);
        }
        return emissive;
    }
    return COLOR_BLACK;
    // vec3 unit_direction = glm::normalize(r.direction());
    // float k = (unit_direction.y + 1.0f) * 0.5f;
    // return LERP(COLOR_WHITE, COLOR_SKY, k);
}