#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <time.h>
#include <vector>
#include <utility>
#include "global.hpp"
#include "image.hpp"
#include "geometry.hpp"
#include "camera.hpp"
#include "material.hpp"
#include "scene.hpp"
#ifdef _OPENMP
#include <omp.h>
#endif

#define WRITE_COLOR(image,x,y,color) do { image(x, y, 0) = color.r;  \
                                          image(x, y, 1) = color.g;  \
                                          image(x, y, 2) = color.b; } while(0)

vec4 ray_color(const Geometry::Ray & r, const Geometry::Hittable & world, int depth);

int main()
{
    // --- Configuration ---
    // modify as needed

    float aspect_ratio = 3.0f / 2.0f;   // aspect ratio (h / w)
    int scr_h = 512;                    // image height
    int samples_per_pixel = 500;        // samples per pixel
    int max_depth = 50;                 // max ray tracing depth
    bool bilinear_filter = false;       // perform bilinear filter to result
    int tile = 8;                       // tiles num
    int scene_idx = 5;                  // which scene to render
    // 0 - random spheres as in 'Ray Tracing in One Weekend'
    // 1 - simpler scene with 3 spheres and 3 emissive triangles as in Ray Tracing in One Weekend
    // 2 - two perlin spheres
    // 3 - earth with texture
    // 4 - cornell box as in 'Ray Tracing the Next Week' (will change aspect_ratio to 1)
    // 5 - cornell box with rotated boxes (will change aspect_ratio to 1)
    // 6 - cornell box with mesh inside (will change aspect_ratio to 1)

    // World
    Geometry::BVH::Node world;
    vec3 eye, at, up;
    float fov;

    switch (scene_idx)
    {
        case 0:
            world = generate_random_scene();
            eye = vec3(13.0f, 2.0f, 3.0f);
            at  = vec3( 0.0f, 0.0f, 0.0f);
            up  = vec3( 0.0f, 1.0f, 0.0f);
            fov = 20.0f;
            break;
        case 1:
            world = generate_simple_scene();
            eye = vec3( 0.0f, 4.0f, 6.0f);
            at  = vec3( 0.0f, 0.0f, 0.0f);
            up  = vec3( 0.0f, 1.0f, 0.0f);
            fov = 90.0f;
            break;
        case 2:
            world = generate_two_perlin_spheres();
            eye = vec3(13.0f, 2.0f, 3.0f);
            at  = vec3( 0.0f, 0.0f, 0.0f);
            up  = vec3( 0.0f, 1.0f, 0.0f);
            fov = 20.0;
            break;
        case 3:
            world = generate_earth();
            eye = vec3(13.0f, 2.0f, 3.0f);
            at  = vec3( 0.0f, 0.0f, 0.0f);
            up  = vec3( 0.0f, 1.0f, 0.0f);
            fov = 20.0;
            break;
        case 4:
            world = generate_cornell_box();
            eye = vec3(278.0f, 278.0f, -750.0f);
            at  = vec3(278.0f, 278.0f,    0.0f);
            up  = vec3(  0.0f,  1.0f,     0.0f);
            fov = 40.0;
            aspect_ratio = 1.0f;
            break;
        case 5:
            world = generate_cornell_box_transformed();
            eye = vec3(278.0f, 278.0f, -750.0f);
            at  = vec3(278.0f, 278.0f,    0.0f);
            up  = vec3(  0.0f,  1.0f,     0.0f);
            fov = 40.0;
            aspect_ratio = 1.0f;
            break;
        case 6:
            world = generate_cornell_box_mesh();
            eye = vec3(278.0f, 278.0f, -750.0f);
            at  = vec3(278.0f, 278.0f,    0.0f);
            up  = vec3(  0.0f,  1.0f,     0.0f);
            fov = 40.0;
            aspect_ratio = 1.0f;
            break;
    }

    int scr_w = static_cast<int>(scr_h * aspect_ratio);
    Utility::Image image(scr_w, scr_h, 3);

    // Camera
    float aperture = 0.1f;
    float focal_length = 10.0f;
    Scene::Camera camera(eye, at, up, fov, aspect_ratio, aperture, focal_length, 1.0f);

    // Render
    clock_t last_timestamp = clock();
    clock_t start_timestamp = last_timestamp;
    float duration;
    char estimate_time[DURATION_STR_LENGTH];
    int w_per_tile = (image.width() + tile - 1) / tile;
    int h_per_tile = (image.height() + tile - 1) / tile;

    // Tiling
    std::vector<std::pair<int, int> > w_tiles;
    std::vector<std::pair<int, int> > h_tiles;
    for (int i = 0; i < tile; i++)
    {
        w_tiles.push_back(std::make_pair<int, int>(i * w_per_tile, MIN((i + 1) * w_per_tile, image.width())));
        h_tiles.push_back(std::make_pair<int, int>(i * h_per_tile, MIN((i + 1) * h_per_tile, image.height())));
    }

    // Rendering
#ifdef _OPENMP
    printf("[INFO] Omp max threads: %d\n", omp_get_max_threads());
#endif
    for (int tj = tile - 1; tj >= 0; tj--)
    for (int ti = 0; ti < tile; ti++)
    {
        for (int j = h_tiles[tj].first; j < h_tiles[tj].second; j++)
        {
#ifdef _OPENMP
#pragma omp parallel for
#endif
            for (int i = w_tiles[ti].first; i < w_tiles[ti].second; i++)
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
            printf("\r[INFO] Rendering tile [%d %d], Scanlines remaining: % 4d, % 5.2f scanlines per second, Estimated time left: %s    ",
                tj, ti, h_tiles[tj].second - j, 1.0 / duration, estimate_time);
            fflush(stdout);
            last_timestamp = clock();
        }
        image.save("result.png");
    }
    
    printf("\n");
    
    char total_time[DURATION_STR_LENGTH];
    duration = ((float)(clock() - start_timestamp) / CLOCKS_PER_SEC);
    get_duration_str(duration, total_time);
    printf("[INFO] Done! Total time: %s\n", total_time);

    // Filtering Image
    if (bilinear_filter)
    {
        auto res = Utility::bilateral_filtering(image, 9, 0.1f, 10.0f);
        res.save("result.png");
    }
    else
    {
        image.save("result.png");
    }

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
    // return COLOR_BLACK;
    vec3 unit_direction = glm::normalize(r.direction());
    float k = (unit_direction.y + 1.0f) * 0.5f;
    return LERP(COLOR_WHITE, COLOR_SKY, k);
}