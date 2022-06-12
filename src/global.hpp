#ifndef __GLOBAL_HPP__
#define __GLOBAL_HPP__

// Linear algebra liberary

#include "thirdparty/glm/glm/glm.hpp"
using vec4 = glm::vec4;
using vec3 = glm::vec3;
using vec2 = glm::vec2;

#define COLOR_WHITE vec4(1.0f, 1.0f, 1.0f, 1.0f)
#define COLOR_BLACK vec4(0.0f, 0.0f, 0.0f, 1.0f);
#define COLOR_RED   vec4(1.0f, 0.0f, 0.0f, 1.0f);
#define COLOR_GREEN vec4(0.0f, 1.0f, 0.0f, 1.0f);
#define COLOR_BLUE  vec4(0.0f, 0.0f, 1.0f, 1.0f);
#define COLOR_CORAL vec4(1.0f, 0.7f, 0.5f, 1.0f);
#define COLOR_SKY   vec4(0.5f, 0.7f, 1.0f, 1.0f);

// Macro functions

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define clamp(x,a,b) min(max(x,a),b)
#define lerp(a,b,c) (a*(1.0f-c)+b*c)

#endif