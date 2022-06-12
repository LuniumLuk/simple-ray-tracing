#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include "global.hpp"
#include "geometry.hpp"

namespace Scene
{

class Camera
{
private:
    vec3 m_origin;
    vec3 m_horizontal;
    vec3 m_vertical;
    vec3 m_lower_left_corner;

public:
    Camera()
    {
        float aspect_ratio = 16.0 / 9.0;
        float viewport_h = 2.0f;
        float viewport_w = aspect_ratio * viewport_h;
        auto focal_length = 1.0;

        m_origin = vec3(0.0f, 0.0f, 0.0f);
        m_horizontal = vec3(viewport_w, 0.0f, 0.0f);
        m_vertical = vec3(0.0f, viewport_h, 0.0f);
        m_lower_left_corner = m_origin - m_horizontal * 0.5f - m_vertical * 0.5f - vec3(0.0f, 0.0f, focal_length);
    }

    Geometry::Ray get_ray(float u, float v) const
    {
        return Geometry::Ray(m_origin, m_lower_left_corner + u * m_horizontal + v * m_vertical - m_origin);
    }
};

}

#endif