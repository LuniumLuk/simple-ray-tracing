#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <stdio.h>
#include <stdlib.h>
#define TINYOBJLOADER_IMPLEMENTATION
#include "thirdparty/tiny_obj_loader.h"
#include "global.hpp"

namespace Utility
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

}

#endif