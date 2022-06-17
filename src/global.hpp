#ifndef __GLOBAL_HPP__
#define __GLOBAL_HPP__

// Linear algebra liberary

#include "thirdparty/glm/glm/glm.hpp"
using vec4 = glm::vec4;
using vec3 = glm::vec3;
using vec2 = glm::vec2;

#define COLOR_WHITE vec4(1.0f, 1.0f, 1.0f, 1.0f)
#define COLOR_BLACK vec4(0.0f, 0.0f, 0.0f, 1.0f)
#define COLOR_RED   vec4(1.0f, 0.0f, 0.0f, 1.0f)
#define COLOR_GREEN vec4(0.0f, 1.0f, 0.0f, 1.0f)
#define COLOR_BLUE  vec4(0.0f, 0.0f, 1.0f, 1.0f)
#define COLOR_CORAL vec4(1.0f, 0.7f, 0.5f, 1.0f)
#define COLOR_SKY   vec4(0.5f, 0.7f, 1.0f, 1.0f)

// Macro functions

#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define CLAMP(x,a,b) MIN(MAX(x,a),b)
#define LERP(a,b,c) (a*(1.0f-c)+b*c)

// Global variables & Utility functions

#include <limits.h>
#include <random>

const float FLOAT_INFINITY = std::numeric_limits<float>::infinity();
const float PI = 3.1415926535897932385f;
const float EPSILON = 1e-6f;


inline float degree_to_radian(float degree)
{
    return degree * PI / 180.0f;
}

inline float random_float()
{
    static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    static std::mt19937 generator;
    return distribution(generator);
}

inline float random_float(float min, float max)
{
    return min + (max - min) * random_float();
}

inline vec3 random_vec3()
{
    return vec3(random_float(), random_float(), random_float());
}

inline vec3 random_vec3(float min, float max)
{
    return vec3(random_float(min, max), random_float(min, max), random_float(min, max));
}

inline bool zero_vec3(const vec3 & v)
{
    return (fabsf(v.x) < EPSILON) && (fabsf(v.y) < EPSILON) && (fabsf(v.z) < EPSILON);
}

vec3 random_unit_vector()
{
    vec3 p = random_vec3(-1.0f, 1.0f);
    return glm::normalize(p);
}

vec3 random_in_unit_sphere()
{
    return random_unit_vector() * random_float();
}

vec3 random_unit_hemisphere(const vec3 & normal) {
    vec3 random_unit = random_unit_vector();
    if (dot(random_unit, normal) > 0)
        return random_unit;
    else
        return -random_unit;
}

vec3 random_in_unit_disk() {
    vec3 p = vec3(random_float(-1.0f, 1.0f), random_float(-1.0f, 1.0f), 0.0f);
    return glm::normalize(p) * random_float();
}

// Timer

#include <string.h>
#define DURATION_STR_LENGTH 24

// duration in seconds
void get_duration_str(float duration, char * buffer)
{
    int hour, minute;
    float second;
    minute = (int)duration / 60;
    second = duration - (float)(minute * 60);
    hour = minute / 60;
    minute %= 60;
    sprintf(buffer, "%02d:%02d:%05.2fs", hour, minute, second);
}


#endif