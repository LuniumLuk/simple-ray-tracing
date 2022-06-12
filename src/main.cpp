#include <stdio.h>
#include <stdlib.h>
#include "global.hpp"
#include "image.hpp"
#include "mesh.hpp"
#include "geometry.hpp"
#include "camera.hpp"

#define COLOR_PRINT(vec) printf("Vector4: (%f, %f, %f, %f)\n", vec.r, vec.g, vec.b, vec.a)
#define WRITE_COLOR(image,x,y,color) do { image.pixel_at(x, y, 0) = color.r;  \
                                          image.pixel_at(x, y, 1) = color.g;  \
                                          image.pixel_at(x, y, 2) = color.b; } while(0)


vec4 ray_color(const Geometry::Ray & r, const Geometry::Hittable & world, int depth)
{
    if (depth <= 0) return COLOR_BLACK;

    Geometry::HitRecord rec;
    if (world.hit(r, 0.001f, FLOAT_INFINITY, rec))
    {
        vec3 target = rec.point + random_unit_hemisphere(rec.normal);
        return 0.5f * ray_color(Geometry::Ray(rec.point, target - rec.point), world, depth - 1);
    }
    vec3 unit_direction = glm::normalize(r.direction());
    unit_direction = 0.5f * (unit_direction + 1.0f);
    return vec4(0.5f * (unit_direction + 1.0f), 1.0f);
}

int main()
{
    // Image

    const float aspect_ratio = 16.0f / 9.0f;
    const int scr_h = 400;
    const int scr_w = static_cast<int>(scr_h * aspect_ratio);
    const int samples_per_pixel = 96;
    const int max_depth = 48;

    Utility::Image image(scr_w, scr_h, 3);

    // World

    Geometry::HittableList world;
    world.add(
        std::make_shared<Geometry::Sphere>(vec3(0.0f, 0.0f, -1.0f), 0.5f));
    world.add(
        std::make_shared<Geometry::Sphere>(vec3(0.0f, -100.5f, -1.0f), 100.0f));

    // Camera

    Scene::Camera camera;

    // Render

    {
        Timer t("render");
        for (int j = image.height() - 1; j >= 0; j--) {
            printf("\r[INFO] Scanlines remaining: %-5d", j);
            fflush(stdout);
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
        }
        printf("\n");
    }
    

    printf("[INFO] Done!\n");

    image.save("result.png");

    return 0;
} 