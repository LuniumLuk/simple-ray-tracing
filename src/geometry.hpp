#ifndef __GEOMETRY_HPP__
#define __GEOMETRY_HPP__

#include "global.hpp"

namespace Geometry
{

class Ray
{
private:
    vec3 m_origin;
    vec3 m_direction;

public:
    Ray() = delete;

    Ray(vec3 origin, vec3 direction):
        m_origin(origin),
        m_direction(direction) {}

    vec3 origin() const { return m_origin; }
    vec3 direction() const { return m_direction; }

    vec3 at(float t) const
    {
        return m_origin + t * m_direction;
    }
};

struct HitRecord
{
    vec3 point;
    vec3 normal;
    float t;
};

class Hittable
{
public:
    virtual bool hit(const Ray & r, float t_min, float t_max, HitRecord & rec) const = 0;
};

class Sphere : public Hittable
{
private:
    vec3 m_center;
    float m_radius;
public:
    Sphere() = delete;

    Sphere(const vec3 & center, float radius):
        m_center(center),
        m_radius(radius) {}
    
    virtual bool hit(const Ray & r, float t_min, float t_max, HitRecord & rec) const override;
};

bool Sphere::hit(const Ray & r, float t_min, float t_max, HitRecord & rec) const
{
    vec3 oc = r.origin() - m_center;
    float a = glm::dot(r.direction(), r.direction());
    float half_b = glm::dot(oc, r.direction());
    float c = glm::dot(oc, oc) - m_radius * m_radius;

    float discriminant = half_b * half_b - a * c;
    if (discriminant < 0) return false;
    float sqrt_d = sqrtf(discriminant);

    // Find the nearest acceptable hit point within the t range
    float t = (-half_b - sqrt_d) / a;
    if (t < t_min || t > t_max)
    {
        t = (-half_b + sqrt_d) / a;
        if (t < t_min || t > t_max) return false;
    }

    rec.t = t;
    rec.point = r.at(t);
    rec.normal = (rec.point - m_center) / m_radius;

    return true;
}


}

#endif