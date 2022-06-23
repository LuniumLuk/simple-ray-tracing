#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

#include "global.hpp"
#include "geometry.hpp"
#include "image.hpp"

using std::make_shared;

namespace Material
{

class Material
{
public:
    virtual bool scatter(
        const Geometry::Ray & r_in, const Geometry::HitRecord & rec, vec4 & attenuation, Geometry::Ray & scattered
    ) const = 0;

    virtual vec4 emitted() const {
        return vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
};

class Lambertian : public Material
{
private:
    shared_ptr<Utility::Texture> m_albedo;

public:
    Lambertian(const vec4 & albedo): 
        m_albedo(make_shared<Utility::SolidColor>(albedo)) {}

    Lambertian(shared_ptr<Utility::Texture> texture): m_albedo(texture) {}

    virtual bool scatter(
        const Geometry::Ray & r_in, const Geometry::HitRecord & rec, vec4 & attenuation, Geometry::Ray & scattered
    ) const override
    {
        vec3 scatter_direction = rec.normal + random_unit_vector();

        // Degenerated direction
        if (zero_vec3(scatter_direction))
        {
            scatter_direction = rec.normal;
        }

        scattered = Geometry::Ray(rec.point, scatter_direction, r_in.time());
        attenuation = m_albedo->value(rec.u, rec.v, rec.point);
        return true;
    }
};

class Metal : public Material
{
private:
    vec4 m_albedo;
    float m_fuzz;

public:
    Metal(const vec4 & albedo, float fuzz): 
        m_albedo(albedo),
        m_fuzz(CLAMP(fuzz, 0.0f, 1.0f)) {}

    virtual bool scatter(
        const Geometry::Ray & r_in, const Geometry::HitRecord & rec, vec4 & attenuation, Geometry::Ray & scattered
    ) const override
    {
        vec3 reflected = glm::reflect(glm::normalize(r_in.direction()), rec.normal);
        scattered = Geometry::Ray(rec.point, reflected + m_fuzz * random_in_unit_sphere(), r_in.time());
        attenuation = m_albedo;
        return (glm::dot(scattered.direction(), rec.normal) > 0);
    }
};

vec3 refract(const vec3 & uv, const vec3 & n, float etai_over_etat) {
    float cos_theta = MIN(glm::dot(-uv, n), 1.0f);
    vec3 r_out_perp =  etai_over_etat * (uv + cos_theta * n);
    vec3 r_out_parallel = -sqrtf(fabsf(1.0f - glm::dot(r_out_perp, r_out_perp))) * n;
    return r_out_perp + r_out_parallel;
}

class Dielectric : public Material
{
private:
    float m_ir;  

    static float reflectance(float cosine, float ref_idx) {
        // Use Schlick's approximation for reflectance.
        float r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * powf((1.0f - cosine), 5.0f);
    }  

public:
    Dielectric(float index_of_refraction):
        m_ir(index_of_refraction) {}

    virtual bool scatter(
        const Geometry::Ray & r_in, const Geometry::HitRecord & rec, vec4 & attenuation, Geometry::Ray & scattered
    ) const override
    {
        attenuation = COLOR_WHITE;
        float refraction_ratio = rec.front_face ? (1.0f / m_ir) : m_ir;

        vec3 unit_direction = glm::normalize(r_in.direction());
        float cos_theta = MIN(glm::dot(-unit_direction, rec.normal), 1.0f);
        float sin_theta = sqrtf(1.0f - cos_theta * cos_theta);

        bool cannot_refract = refraction_ratio * sin_theta > 1.0f;
        vec3 direction;

        if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_float())
            direction = reflect(unit_direction, rec.normal);
        else
            direction = refract(unit_direction, rec.normal, refraction_ratio);

        scattered = Geometry::Ray(rec.point, direction, r_in.time());
        return true;
    }
};

class Emissive : public Material
{
private:
    vec4 m_color;

public:
    Emissive(const vec4 & color): 
        m_color(color) {}

    virtual bool scatter(
        const Geometry::Ray & r_in, const Geometry::HitRecord & rec, vec4 & attenuation, Geometry::Ray & scattered
    ) const override
    {
        return false;
    }

    virtual vec4 emitted() const override {
        return m_color;
    }
};

}

#endif