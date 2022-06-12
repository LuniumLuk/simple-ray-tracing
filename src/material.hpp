#ifndef __MATERIAL_HPP__
#define __MATERIAL_HPP__

#include "global.hpp"

namespace Geometry { struct HitRecord; class Ray; }

namespace Material
{

class Material
{
public:
    virtual bool scatter(
        const Geometry::Ray & r_in, const Geometry::HitRecord & rec, vec4 & attenuation, Geometry::Ray & scattered
    ) const = 0;
};

}


#endif