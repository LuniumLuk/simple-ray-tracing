#ifndef __GEOMETRY_HPP__
#define __GEOMETRY_HPP__

#include "global.hpp"
#include <memory>

namespace Material { class Material; }

namespace Geometry
{

class Ray
{
private:
    vec3 m_origin;
    vec3 m_direction;

public:
    Ray() = default;

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
    std::shared_ptr<Material::Material> material;
    float t;
    bool front_face;

    inline void set_face_normal(const Ray & r, const vec3 & outward_normal) {
        front_face = glm::dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
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
    std::shared_ptr<Material::Material> m_material;

public:
    Sphere() = delete;

    Sphere(const vec3 & center, float radius, std::shared_ptr<Material::Material> material):
        m_center(center),
        m_radius(radius),
        m_material(material) {}
    
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
    vec3 outward_normal = (rec.point - m_center) / m_radius;
    rec.set_face_normal(r, outward_normal);
    rec.material = m_material;

    return true;
}

class HittableList : public Hittable
{
private:
    std::vector<std::shared_ptr<Hittable> > m_objects;

public:
    HittableList() {}

    void clear() { m_objects.clear(); }
    void add(std::shared_ptr<Hittable> object)
    {
        m_objects.push_back(object);
    }

    virtual bool hit(const Ray & r, float t_min, float t_max, HitRecord & rec) const override;
};

bool HittableList::hit(const Ray & r, float t_min, float t_max, HitRecord & rec) const
{
    HitRecord temp_rec;
    bool is_hit = false;
    float t_closest = t_max;

    for (int i = 0; i < m_objects.size(); i++)
    {
        if (m_objects[i]->hit(r, t_min, t_closest, temp_rec))
        {
            is_hit = true;
            t_closest = temp_rec.t;
            rec = temp_rec;
        }
    }

    return is_hit;
}


}

#endif