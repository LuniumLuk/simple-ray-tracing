#ifndef __GLOBAL_HPP__
#define __GLOBAL_HPP__

// Linear algebra liberary

#include "thirdparty/glm/glm.hpp"
using vec4 = glm::vec4;
using vec3 = glm::vec3;
using vec2 = glm::vec2;

// Macro functions

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define clamp(x,a,b) min(max(x,a),b)
#define lerp(a,b,c) (a*(1.0f-c)+b*c)

#endif