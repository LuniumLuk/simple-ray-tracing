
#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <stdio.h>
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glm/glm.hpp"

namespace Utils
{

class Image
{
private:
    float *m_data;
    int m_width;
    int m_height;
    int m_channels;

public:
    Image(): 
        m_data(nullptr), 
        m_width(0),
        m_height(0),
        m_channels(0) {}
    
    Image(const char * filename)
    {
        m_data = stbi_loadf(filename, &m_width, &m_height, &m_channels, 0);
        if (!m_data)
        {
            printf("[ERROR] Failed to load [%s]\n", filename);
            exit(-1);
        }
    }

    /**
     * Write to image data at certain position
     */
    const void write(const char * filename) const;

    /**
     * Sample a rgba color with a float uv coords
     */
    glm::vec4 sample(const glm::vec2 & uv) const;

    /**
     * Access to image data at certain integer position
     */
    float & operator() (int x, int y, int c);

    const int width() const { return m_width; }
    const int height() const { return m_height; }
    const int channels() const { return m_channels; }
};

}

#endif