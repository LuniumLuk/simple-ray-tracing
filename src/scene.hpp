#ifndef __SCENE_HPP__
#define __SCENE_HPP__

#include "global.hpp"
#include "geometry.hpp"
#include "material.hpp"

using std::make_shared;
using std::shared_ptr;

#define RANDOM_COLOR() random_vec3()

Geometry::BVH::Node generate_random_scene() {
    Geometry::HittableList world;

    // auto ground_material = make_shared<Material::Lambertian>(vec4(0.5f, 0.5f, 0.5f, 1.0f));
    auto checker = make_shared<Utility::CheckerTexture>(vec4(0.2f, 0.3f, 0.1f, 1.0f), vec4(0.9f, 0.9f, 0.9f, 1.0f));
    auto ground_material = make_shared<Material::Lambertian>(checker);
    world.add(make_shared<Geometry::Sphere>(vec3(0.0f, -1000.0f, 0.0f), 1000.0f, ground_material));

    for (int a = -11; a < 11; a++)
    {
        for (int b = -11; b < 11; b++)
        {
            float material_choice = random_float();
            vec3 center(a + 0.9f * random_float(), 0.2f, b + 0.9f * random_float());

            if (glm::length(center - vec3(4.0f, 0.2f, 0.0f)) > 0.9f) {
                shared_ptr<Material::Material> sphere_material;

                if (material_choice < 0.8f)
                {
                    // diffuse
                    vec4 albedo = vec4(RANDOM_COLOR() * RANDOM_COLOR(), 1.0f);
                    sphere_material = make_shared<Material::Lambertian>(albedo);
                    vec3 center1 = center + vec3(0, random_float(0, 0.5f), 0);
                    world.add(make_shared<Geometry::Sphere>(center, 0.2f, sphere_material));
                    // world.add(make_shared<Geometry::MovingSphere>(center, center1, 0.0f, 1.0f, 0.2f, sphere_material));
                }
                else if (material_choice < 0.95)
                {
                    // metal
                    vec4 albedo = vec4(RANDOM_COLOR() * 0.5f + 0.5f, 1.0f);
                    float fuzz = random_float(0.0f, 0.5f);
                    sphere_material = make_shared<Material::Metal>(albedo, fuzz);
                    world.add(make_shared<Geometry::Sphere>(center, 0.2f, sphere_material));
                }
                else
                {
                    // glass
                    sphere_material = make_shared<Material::Dielectric>(1.5f);
                    world.add(make_shared<Geometry::Sphere>(center, 0.2f, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<Material::Dielectric>(1.5);
    auto material2 = make_shared<Material::Lambertian>(vec4(0.4f, 0.2f, 0.1f, 1.0f));
    auto material3 = make_shared<Material::Metal>(vec4(0.7f, 0.6f, 0.5f, 1.0f), 0.0f);

    // world.add(make_shared<Geometry::Triangle>(
    //     vec3( 0.0f, 2.0f, 2.0f),
    //     vec3(-1.0f, 1.0f, 0.0f),
    //     vec3(-2.0f, 0.0f, 2.0f),
    //     ground_material
    // ));

    world.add(make_shared<Geometry::Sphere>(vec3( 0.0f, 1.0f, 0.0f), 1.0f, material1));
    world.add(make_shared<Geometry::Sphere>(vec3(-4.0f, 1.0f, 0.0f), 1.0f, material2));
    world.add(make_shared<Geometry::Sphere>(vec3( 4.0f, 1.0f, 0.0f), 1.0f, material3));

    return Geometry::BVH::Node(world, 0.0f, 1.0f);
}

Geometry::BVH::Node generate_simple_scene() {
    Geometry::HittableList world;

    auto ground_material = make_shared<Material::Lambertian>(vec4(0.5f, 0.5f, 0.5f, 1.0f));
    world.add(make_shared<Geometry::Sphere>(vec3(0.0f, -1000.0f, 0.0f), 1000.0f, ground_material));

    auto material1 = make_shared<Material::Dielectric>(1.5);
    auto material2 = make_shared<Material::Lambertian>(vec4(0.4f, 0.2f, 0.1f, 1.0f));
    auto material3 = make_shared<Material::Metal>(vec4(0.7f, 0.6f, 0.5f, 1.0f), 0.0f);
    world.add(make_shared<Geometry::Sphere>(vec3( 0.0f, 1.0f, 0.0f),  1.0f, material1));
    world.add(make_shared<Geometry::Sphere>(vec3( 0.0f, 1.0f, 0.0f), -0.9f, material1));
    world.add(make_shared<Geometry::Sphere>(vec3(-2.0f, 1.0f, 2.0f), 1.0f, material1));
    world.add(make_shared<Geometry::Sphere>(vec3( 2.0f, 1.0f, 2.0f), 1.0f, material1));

    auto material4 = make_shared<Material::Emissive>(vec4(1.0f, 1.0f, 1.0f, 1.0f));
    world.add(make_shared<Geometry::Triangle>(
        vec3(-3.0f, 0.0f, -2.0f),
        vec3( 0.0f, 4.0f, -2.0f),
        vec3( 3.0f, 0.0f, -2.0f),
        material4));
    world.add(make_shared<Geometry::Triangle>(
        vec3(-4.0f, 0.0f, 0.0f),
        vec3(-4.0f, 4.0f, 0.0f),
        vec3(-4.0f, 0.0f, 4.0f),
        material4));
    world.add(make_shared<Geometry::Triangle>(
        vec3( 4.0f, 0.0f, 0.0f),
        vec3( 4.0f, 4.0f, 0.0f),
        vec3( 4.0f, 0.0f, 4.0f),
        material4));

    return Geometry::BVH::Node(world, 0.0f, 1.0f);
}

Geometry::BVH::Node generate_two_perlin_spheres()
{
    Geometry::HittableList world;

    auto pertext = make_shared<Utility::NoiseTexturePos>(4.0f);
    world.add(make_shared<Geometry::Sphere>(vec3(0.0f, -1000.0f, 0.0f), 1000.0f, make_shared<Material::Lambertian>(pertext)));
    world.add(make_shared<Geometry::Sphere>(vec3(0.0f,     2.0f, 0.0f),    2.0f, make_shared<Material::Lambertian>(pertext)));

    return Geometry::BVH::Node(world, 0.0f, 1.0f);
}

Geometry::BVH::Node generate_earth()
{
    Geometry::HittableList world;

    auto earth_tex = make_shared<Utility::ImageTexture>("assets/texture/earthmap.jpg");
    world.add(make_shared<Geometry::Sphere>(vec3(0.0f, 0.0f, 0.0f), 2.0f, make_shared<Material::Lambertian>(earth_tex)));

    return Geometry::BVH::Node(world, 0.0f, 1.0f);
}


#endif