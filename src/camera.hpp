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

    vec3 m_u, m_v, m_w;
    float m_lens_radius;

public:
    Camera(
        vec3 eye,
        vec3 at,
        vec3 up,
        float vertical_fov,
        float aspect_ratio,
        float aperture,
        float focal_lenght
    )
    {
        float theta = degree_to_radian(vertical_fov);
        float h = tanf(theta * 0.5f);
        float viewport_h = 2.0f * h;
        float viewport_w = aspect_ratio * viewport_h;
        
        m_w = glm::normalize(eye - at);
        m_u = glm::normalize(glm::cross(up, m_w));
        m_v = glm::cross(m_w, m_u);

        m_origin = eye;
        m_horizontal = focal_lenght * viewport_w * m_u;
        m_vertical = focal_lenght * viewport_h * m_v;
        m_lower_left_corner = m_origin - m_horizontal * 0.5f - m_vertical * 0.5f - focal_lenght * m_w;

        m_lens_radius = aperture * 0.5f;
    }

    Geometry::Ray get_ray(float s, float t) const
    {
        vec3 rd = m_lens_radius * random_in_unit_disk();
        vec3 offset = m_u * rd.x + m_v * rd.y;

        return Geometry::Ray(
            m_origin + offset, 
            m_lower_left_corner + s * m_horizontal + t * m_vertical - m_origin - offset
        );
    }
};

}

#endif