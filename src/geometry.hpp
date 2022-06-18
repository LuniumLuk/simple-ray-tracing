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
    float m_time;

public:
    Ray() = default;

    Ray(vec3 origin, vec3 direction, float time):
        m_origin(origin),
        m_direction(direction),
        m_time(time) {}

    vec3 origin() const { return m_origin; }
    vec3 direction() const { return m_direction; }
    float time() const { return m_time; }

    vec3 at(float t) const
    {
        return m_origin + t * m_direction;
    }
};

class AABB
{
private:
    vec3 m_min;
    vec3 m_max;

public:
    AABB() {}

    AABB(const vec3 & min, const vec3 & max):
        m_min(min),
        m_max(max) {}

    vec3 min() const { return m_min; }
    vec3 max() const { return m_max; }

    bool hit(const Ray & r, double t_min, double t_max) const
    {
        for (int a = 0; a < 3; a++)
        {
            float invD = 1.0f / r.direction()[a];
            float t0 = (min()[a] - r.origin()[a]) * invD;
            float t1 = (max()[a] - r.origin()[a]) * invD;
            if (invD < 0)
                std::swap(t0, t1);
            t_min = fmax(t0, t_min);
            t_max = fmin(t1, t_max);
            if (t_max <= t_min)
                return false;
        }
        return true;
    }
};

AABB surrounding_box(AABB box0, AABB box1) {
    vec3 min(fmin(box0.min().x, box1.min().x),
             fmin(box0.min().y, box1.min().y),
             fmin(box0.min().z, box1.min().z));

    vec3 max(fmax(box0.max().x, box1.max().x),
             fmax(box0.max().y, box1.max().y),
             fmax(box0.max().z, box1.max().z));

    return AABB(min, max);
}

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
    virtual bool bounding_box(float time0, float time1, AABB & output_box) const = 0;
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
    virtual bool bounding_box(float time0, float time1, AABB & output_box) const override;
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

bool Sphere::bounding_box(float time0, float time1, AABB & output_box) const
{
    output_box = AABB(
        m_center - vec3(m_radius, m_radius, m_radius),
        m_center + vec3(m_radius, m_radius, m_radius));
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
    const std::vector<std::shared_ptr<Hittable> > & objects() const { return m_objects; }

    virtual bool hit(const Ray & r, float t_min, float t_max, HitRecord & rec) const override;
    virtual bool bounding_box(float time0, float time1, AABB & output_box) const override;
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

bool HittableList::bounding_box(float time0, float time1, AABB & output_box) const
{
    if (m_objects.empty()) return false;

    AABB temp_box;
    bool first_box = true;

    for (int i = 0; i < m_objects.size(); i++)
    {
        if (!m_objects[i]->bounding_box(time0, time1, temp_box))
        {
            return false;
        }
        output_box = first_box ? temp_box : surrounding_box(output_box, temp_box);
        first_box = false;
    }

    return true;
}

class MovingSphere : public Hittable
{
private:
    vec3 m_center0, m_center1;
    float m_time0, m_time1;
    float m_radius;
    std::shared_ptr<Material::Material> m_material;

public:
    MovingSphere() = delete;

    MovingSphere(
        const vec3 & center0, const vec3 & center1,
        float time0, float time1,
        float radius, std::shared_ptr<Material::Material> material
    ):
        m_center0(center0),
        m_center1(center1),
        m_time0(time0),
        m_time1(time1),
        m_radius(radius),
        m_material(material) {}
    
    vec3 center(float time) const;
    
    virtual bool hit(const Ray & r, float t_min, float t_max, HitRecord & rec) const override;
    virtual bool bounding_box(float time0, float time1, AABB & output_box) const override;
};

vec3 MovingSphere::center(float time) const
{
    return m_center0 + ((time - m_time0) / (m_time1 - m_time0)) * (m_center1 - m_center0);
}

bool MovingSphere::bounding_box(float time0, float time1, AABB & output_box) const
{
    AABB box0 = AABB(
        center(time0) - vec3(m_radius, m_radius, m_radius),
        center(time0) + vec3(m_radius, m_radius, m_radius));
    AABB box1 = AABB(
        center(time1) - vec3(m_radius, m_radius, m_radius),
        center(time1) + vec3(m_radius, m_radius, m_radius));
    output_box = surrounding_box(box0, box1);
    return true;
}

bool MovingSphere::hit(const Ray & r, float t_min, float t_max, HitRecord & rec) const
{
    vec3 oc = r.origin() - center(r.time());
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
    vec3 outward_normal = (rec.point - center(r.time())) / m_radius;
    rec.set_face_normal(r, outward_normal);
    rec.material = m_material;

    return true;
}

class Triangle : public Hittable
{
private:
    vec3 m_v0, m_v1, m_v2;
    vec3 m_normal;
    std::shared_ptr<Material::Material> m_material;

public:
    Triangle() = delete;

    Triangle(
        const vec3 & v0,
        const vec3 & v1,
        const vec3 & v2,
        std::shared_ptr<Material::Material> material
    ):
        m_v0(v0),
        m_v1(v1),
        m_v2(v2),
        m_material(material)
    {
        m_normal = glm::cross(v1 - v0, v2 - v0);
    }
    
    vec3 normal() const { return m_normal; }
    
    virtual bool hit(const Ray & r, float t_min, float t_max, HitRecord & rec) const override;
    virtual bool bounding_box(float time0, float time1, AABB & output_box) const override;
};

bool Triangle::hit(const Ray & r, float t_min, float t_max, HitRecord & rec) const
{
    vec3 v01 = m_v1 - m_v0;
    vec3 v12 = m_v2 - m_v1;
    vec3 v20 = m_v0 - m_v2;

    // 1. determine whether the ray is parellel to the triangle
    float N_dot_direction = glm::dot(m_normal, r.direction());
    if (fabs(N_dot_direction) < EPSILON)
    {
        return false;
    }

    // 2. determin whether the triangle is at opposite direction of the ray
    float d = -glm::dot(m_normal, m_v0);
    float t = -(glm::dot(m_normal, r.origin()) + d) / N_dot_direction;
    if (t < t_min || t > t_max)
    {
        return false;
    }

    // 3. get the intersection point
    vec3 P = r.at(t);

    // 4. inside-out test for P
    vec3 inside_out_test;

    vec3 v0P = P - m_v0;
    inside_out_test = glm::cross(v01, v0P);
    if (glm::dot(m_normal, inside_out_test) < 0)
    {
        return false;
    }

    vec3 v1P = P - m_v1;
    inside_out_test = glm::cross(v12, v1P);
    if (glm::dot(m_normal, inside_out_test) < 0)
    {
        return false;
    }

    vec3 v2P = P - m_v2;
    inside_out_test = glm::cross(v20, v2P);
    if (glm::dot(m_normal, inside_out_test) < 0)
    {
        return false;
    }

    rec.t = t;
    rec.point = P;
    vec3 outward_normal = m_normal;
    rec.set_face_normal(r, outward_normal);
    rec.material = m_material;
    
    return true;
}

bool Triangle::bounding_box(float time0, float time1, AABB & output_box) const
{
    vec3 min = vec3(MIN(MIN(m_v0.x, m_v1.x), m_v2.x),
                    MIN(MIN(m_v0.y, m_v1.y), m_v2.y),
                    MIN(MIN(m_v0.z, m_v1.z), m_v2.z));
    vec3 max = vec3(MAX(MAX(m_v0.x, m_v1.x), m_v2.x),
                    MAX(MAX(m_v0.y, m_v1.y), m_v2.y),
                    MAX(MAX(m_v0.z, m_v1.z), m_v2.z));

    output_box = AABB(min, max);
    return true;
}

namespace BVH {

class Node : public Hittable
{
private:
    std::shared_ptr<Hittable> m_left;
    std::shared_ptr<Hittable> m_right;
    AABB m_box;

public:
    Node() = delete;

    Node(const HittableList & list, float time0, float time1):
        Node(list.objects(), 0, list.objects().size(), time0, time1) {}

    Node(const std::vector<std::shared_ptr<Hittable> > & src_objects,
         size_t start, size_t end, float time0, float time1);

    virtual bool hit(const Ray & r, float t_min, float t_max, HitRecord & rec) const override;
    virtual bool bounding_box(float time0, float time1, AABB & output_box) const override;
};

bool Node::bounding_box(float time0, float time1, AABB & output_box) const
{
    output_box = m_box;
    return true;
}

bool Node::hit(const Ray & r, float t_min, float t_max, HitRecord& rec) const
{
    if (!m_box.hit(r, t_min, t_max))
        return false;

    bool hit_left = m_left->hit(r, t_min, t_max, rec);
    bool hit_right = m_right->hit(r, t_min, hit_left ? rec.t : t_max, rec);

    return hit_left || hit_right;
}

} // namespace BVH

} // namespace Geometry

#endif