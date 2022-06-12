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

#include <limits>
#include <random>

const float FLOAT_INFINITY = std::numeric_limits<float>::infinity();
const float PI = 3.1415926535897932385f;

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

vec3 random_unit_vector()
{
    vec3 p = random_vec3(-1.0f, 1.0f);
    return glm::normalize(p);
}

vec3 random_unit_hemisphere(const vec3 & normal) {
    vec3 random_unit = random_unit_vector();
    if (dot(random_unit, normal) > 0)
        return random_unit;
    else
        return -random_unit;
}

// Timer

#include <stdio.h>
#include <time.h>

class Timer
{
private:
    timespec    m_timer_start;
    char*       m_timer_name;

public:
    Timer()
    {
        m_timer_name = new char[8];
        strcpy(m_timer_name, "Default");

        clock_gettime(CLOCK_REALTIME, &m_timer_start);
    }
    Timer(const char* name)
    {
        m_timer_name = new char[strlen(name) + 1];
        strcpy(m_timer_name, name);

        clock_gettime(CLOCK_REALTIME, &m_timer_start);
    }

    ~Timer()
    {
        tick();
        delete[] m_timer_name;
    }

    void tick() {
        timespec timer_end;
        clock_gettime(CLOCK_REALTIME, &timer_end);

        double duration;
        if (timer_end.tv_nsec > m_timer_start.tv_nsec)
        {
            duration = (timer_end.tv_sec - m_timer_start.tv_sec - 1) + (double)(1e9 + timer_end.tv_nsec - m_timer_start.tv_nsec) / 1e9;
        }
        else
        {
            duration = (timer_end.tv_sec - m_timer_start.tv_sec) + (double)(timer_end.tv_nsec - m_timer_start.tv_nsec) / 1e9;
        }
        printf("[INFO] Timer (%s) : %.6fs (%.4fms)\n", m_timer_name, duration, duration * 1e3);
    }
};

#endif