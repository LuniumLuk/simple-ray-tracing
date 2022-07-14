#ifndef __RECT_HPP__
#define __RECT_HPP__

#include "global.hpp"
#include "geometry.hpp"
#include "material.hpp"

using std::shared_ptr;
#define RECT_AABB_THICK 0.001f

namespace Geometry 
{

enum AxisAlignedRectType {
    RECT_XY, RECT_XZ, RECT_YZ,
};

class AxisAlignedRect : public Hittable
{
private:
    float m_a0, m_a1;
    float m_b0, m_b1;
    float m_k;
    shared_ptr<Material::Material> m_material;
    AxisAlignedRectType m_type;

public:
    AxisAlignedRect() = delete;

    AxisAlignedRect(float a0, float a1, float b0, float b1, float k, AxisAlignedRectType type, shared_ptr<Material::Material> mat):
        m_a0(a0), m_a1(a1), m_b0(b0), m_b1(b1), m_k(k), m_type(type), m_material(mat) {}


    virtual bool hit(const Ray & r, float t_min, float t_max, HitRecord & rec) const override;
    virtual bool bounding_box(float time0, float time1, AABB & output_box) const override;
};

bool AxisAlignedRect::bounding_box(float time0, float time1, AABB & output_box) const
{
    switch (m_type)
    {
        case RECT_XY:
            output_box = AABB(vec3(m_a0, m_b0, m_k - RECT_AABB_THICK), vec3(m_a1, m_b1, m_k + RECT_AABB_THICK));
            break;
        case RECT_XZ:
            output_box = AABB(vec3(m_a0, m_k - RECT_AABB_THICK, m_b0), vec3(m_a1, m_k + RECT_AABB_THICK, m_b1));
            break;
        case RECT_YZ:
            output_box = AABB(vec3(m_k - RECT_AABB_THICK, m_a0, m_b0), vec3(m_k + RECT_AABB_THICK, m_a1, m_b1));
            break;

    }
    return true;
}

bool AxisAlignedRect::hit(const Ray & r, float t_min, float t_max, HitRecord & rec) const
{
    float t;
    switch (m_type)
    {
        case RECT_XY:
            t = (m_k - r.origin().z) / r.direction().z; break;
        case RECT_XZ:
            t = (m_k - r.origin().y) / r.direction().y; break;
        case RECT_YZ:
            t = (m_k - r.origin().x) / r.direction().x; break;
    }

    if (t < t_min || t > t_max)
    {
        return false;
    }

    float a, b;
    switch (m_type)
    {
        case RECT_XY:
            a = r.origin().x + t * r.direction().x;
            b = r.origin().y + t * r.direction().y;
            break;
        case RECT_XZ:
            a = r.origin().x + t * r.direction().x;
            b = r.origin().z + t * r.direction().z;
            break;
        case RECT_YZ:
            a = r.origin().y + t * r.direction().y;
            b = r.origin().z + t * r.direction().z;
            break;
    }

    if (a < m_a0 || a > m_a1 || b < m_b0 || b > m_b1)
    {
        return false;
    }

    rec.u = (a - m_a0) / (m_a1 - m_a0);
    rec.v = (b - m_b0) / (m_b1 - m_b0);
    rec.t = t;
    vec3 outward_normal;
    switch (m_type)
    {
        case RECT_XY:
            outward_normal = vec3(0.0f, 0.0f, 1.0f); break;
        case RECT_XZ:
            outward_normal = vec3(0.0f, 1.0f, 0.0f); break;
        case RECT_YZ:
            outward_normal = vec3(1.0f, 0.0f, 0.0f); break;
    }
    rec.set_face_normal(r, outward_normal);
    rec.material = m_material;
    rec.point = r.at(t);
    return true;
}

class Box : public Hittable
{
private:
    vec3 m_min;
    vec3 m_max;
    HittableList m_sides;

public:
    Box() = delete;
    Box(const vec3 & min, const vec3 & max, shared_ptr<Material::Material> mat);
    
    virtual bool hit(const Ray & r, float t_min, float t_max, HitRecord & rec) const override;
    virtual bool bounding_box(float time0, float time1, AABB & output_box) const override;
};

Box::Box(const vec3 & min, const vec3 & max, shared_ptr<Material::Material> mat):
    m_min(min), m_max(max)
{
    m_sides.add(make_shared<AxisAlignedRect>(min.x, max.x, min.y, max.y, min.z, AxisAlignedRectType::RECT_XY, mat));
    m_sides.add(make_shared<AxisAlignedRect>(min.x, max.x, min.y, max.y, max.z, AxisAlignedRectType::RECT_XY, mat));

    m_sides.add(make_shared<AxisAlignedRect>(min.x, max.x, min.z, max.z, min.y, AxisAlignedRectType::RECT_XZ, mat));
    m_sides.add(make_shared<AxisAlignedRect>(min.x, max.x, min.z, max.z, max.y, AxisAlignedRectType::RECT_XZ, mat));

    m_sides.add(make_shared<AxisAlignedRect>(min.y, max.y, min.z, max.z, min.x, AxisAlignedRectType::RECT_YZ, mat));
    m_sides.add(make_shared<AxisAlignedRect>(min.y, max.y, min.z, max.z, max.x, AxisAlignedRectType::RECT_YZ, mat));
}

bool Box::hit(const Ray & r, float t_min, float t_max, HitRecord & rec) const
{
    return m_sides.hit(r, t_min, t_max, rec);
}

bool Box::bounding_box(float time0, float time1, AABB & output_box) const
{
    output_box = AABB(m_min, m_max);
    return true;
}


} // namespace Geometry

#endif