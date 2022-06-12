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

#define COLOR_PRINT(vec) printf("Vector4: (%f, %f, %f, %f)\n", vec.r, vec.g, vec.b, vec.a)
#define WRITE_COLOR(image,x,y,color) do { image.pixel_at(x, y, 0) = color.r;  \
                                          image.pixel_at(x, y, 1) = color.g;  \
                                          image.pixel_at(x, y, 2) = color.b; } while(0)
#define RANDOM_COLOR() random_vec3()

Geometry::HittableList generate_random_scene() {
    Geometry::HittableList world;

    auto ground_material = std::make_shared<Material::Lambertian>(vec4(0.5f, 0.5f, 0.5f, 1.0f));
    world.add(std::make_shared<Geometry::Sphere>(vec3(0.0f, -1000.0f, 0.0f), 1000.0f, ground_material));

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            float material_choice = random_float();
            vec3 center(a + 0.9f * random_float(), 0.2f, b + 0.9f * random_float());

            if (glm::length(center - vec3(4.0f, 0.2f, 0.0f)) > 0.9f) {
                std::shared_ptr<Material::Material> sphere_material;

                if (material_choice < 0.8f)
                {
                    // diffuse
                    vec4 albedo = vec4(RANDOM_COLOR() * RANDOM_COLOR(), 1.0f);
                    sphere_material = std::make_shared<Material::Lambertian>(albedo);
                    world.add(std::make_shared<Geometry::Sphere>(center, 0.2, sphere_material));
                }
                else if (material_choice < 0.95)
                {
                    // metal
                    vec4 albedo = vec4(RANDOM_COLOR() * 0.5f + 0.5f, 1.0f);
                    float fuzz = random_float(0.0f, 0.5f);
                    sphere_material = std::make_shared<Material::Metal>(albedo, fuzz);
                    world.add(std::make_shared<Geometry::Sphere>(center, 0.2f, sphere_material));
                }
                else
                {
                    // glass
                    sphere_material = std::make_shared<Material::Dielectric>(1.5f);
                    world.add(std::make_shared<Geometry::Sphere>(center, 0.2f, sphere_material));
                }
            }
        }
    }

    auto material1 = std::make_shared<Material::Dielectric>(1.5);
    world.add(std::make_shared<Geometry::Sphere>(vec3(0.0f, 1.0f, 0.0f), 1.0f, material1));

    auto material2 = std::make_shared<Material::Lambertian>(vec4(0.4f, 0.2f, 0.1f, 1.0f));
    world.add(std::make_shared<Geometry::Sphere>(vec3(-4.0f, 1.0f, 0.0f), 1.0f, material2));

    auto material3 = std::make_shared<Material::Metal>(vec4(0.7f, 0.6f, 0.5f, 1.0f), 0.0f);
    world.add(std::make_shared<Geometry::Sphere>(vec3(4.0f, 1.0f, 0.0f), 1.0f, material3));

    return world;
}

vec4 ray_color(const Geometry::Ray & r, const Geometry::Hittable & world, int depth)
{
    if (depth <= 0) return COLOR_BLACK;

    Geometry::HitRecord rec;
    if (world.hit(r, 0.001f, FLOAT_INFINITY, rec))
    {
        Geometry::Ray scattered;
        vec4 attenuation;
        if (rec.material->scatter(r, rec, attenuation, scattered))
        {
            return attenuation * ray_color(scattered, world, depth - 1);
        }
        return COLOR_BLACK;
    }
    vec3 unit_direction = glm::normalize(r.direction());
    unit_direction = 0.5f * (unit_direction + 1.0f);
    return vec4(0.5f * (unit_direction + 1.0f), 1.0f);
}


int main()
{
    // Image

    const float aspect_ratio = 3.0f / 2.0f;
    const int scr_h = 800;
    const int scr_w = static_cast<int>(scr_h * aspect_ratio);
    const int samples_per_pixel = 256;
    const int max_depth = 48;

    Utility::Image image(scr_w, scr_h, 3);

    // World

    Geometry::HittableList world = generate_random_scene();

#if 0
    auto material_ground = std::make_shared<Material::Lambertian>(vec4(0.8f, 0.8f, 0.0f, 1.0f));
    auto material_center = std::make_shared<Material::Lambertian>(vec4(0.1f, 0.2f, 0.5f, 1.0f));
    auto material_left = std::make_shared<Material::Dielectric>(1.5f);
    auto material_right = std::make_shared<Material::Metal>(vec4(0.8f, 0.6f, 0.2f, 1.0f), 1.0f);

    world.add(std::make_shared<Geometry::Sphere>(vec3( 0.0f, -100.5f, -1.0f), 100.0f, material_ground));
    world.add(std::make_shared<Geometry::Sphere>(vec3( 0.0f,    0.0f, -1.0f),   0.5f, material_center));
    world.add(std::make_shared<Geometry::Sphere>(vec3(-1.0f,    0.0f, -1.0f),   0.5f, material_left));
    world.add(std::make_shared<Geometry::Sphere>(vec3(-1.0f,    0.0f, -1.0f),  -0.4f, material_left));
    world.add(std::make_shared<Geometry::Sphere>(vec3( 1.0f,    0.0f, -1.0f),   0.5f, material_right));
#endif

    // Camera

#if 0
    vec3 eye = vec3(3.0f, 3.0f,  2.0f);
    vec3 at  = vec3(0.0f, 0.0f, -1.0f);
    vec3 up  = vec3(0.0f, 1.0f,  0.0f);
    float aperture = 2.0f;
    float focal_length = glm::length(eye - at);
#endif
    vec3 eye = vec3(13.0f, 2.0f, 3.0f);
    vec3 at  = vec3( 0.0f, 0.0f, 0.0f);
    vec3 up  = vec3( 0.0f, 1.0f, 0.0f);
    float aperture = 0.1f;
    float focal_length = 10.0f;

    Scene::Camera camera(eye, at, up, 20.0f, aspect_ratio, aperture, focal_length);

    // Render
    {
        Timer t("render");
        clock_t last_timestamp = clock();
        float duration;
        long estimate_time;
        long minute, second;
        for (int j = image.height() - 1; j >= 0; j--) {
            for (int i = 0; i < image.width(); ++i) {
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
            estimate_time = duration * j;
            second = estimate_time % 60; estimate_time /= 60;
            minute = estimate_time % 60; estimate_time /= 60;
            printf("\r[INFO] Scanlines remaining: %-5d, Estimated time left: %02ld:%02ld:%02ld", j, estimate_time, minute, second);
            fflush(stdout);
            last_timestamp = clock();
        }
        printf("\n");
    }
    printf("[INFO] Done!\n");

    image.save("result.png");

    return 0;
} 