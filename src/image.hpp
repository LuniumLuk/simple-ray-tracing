#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "thirdparty/stb/stb_image_write.h"
#include "global.hpp"

namespace Utility
{

#define DEFAULT_JPEG_QUALITY 100

class Image
{
private:
    float *m_data;
    int m_width;
    int m_height;
    int m_channels;

    bool ends_with(const char * str, const char * suffix)
    {
        if (!str || !suffix)
            return false;
        size_t lenstr = strlen(str);
        size_t lensuffix = strlen(suffix);
        if (lensuffix > lenstr)
            return false;
        return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
    }

    stbi_uc * image_get_uchar_data()
    {
        float * f_data = (float *)malloc(size() * sizeof(float));
        memcpy(f_data, m_data, size() * sizeof(float));
        return stbi__hdr_to_ldr(f_data, m_width, m_height, m_channels);
    }

public:
    Image(): 
        m_data(nullptr), 
        m_width(0),
        m_height(0),
        m_channels(0) {}

    Image(int width, int height, int channels)
    {
        m_width = width;
        m_height = height;
        m_channels = channels;
        m_data = new float[width * height * channels];
        memset(m_data, 0, width * height * channels * sizeof(float));
    }

    Image(const char * filename): m_data(nullptr)
    {
        load(filename);
    }

    ~Image()
    {
        delete[] m_data;
    }

    void load(const char * filename)
    {
        int width, height, channels;
        float * data = stbi_loadf(filename, &width, &height, &channels, 0);
        if (!data)
        {
            printf("[ERROR] Failed to load [%s]\n", filename);
            exit(-1);
        }
        else if (channels != 3 && channels != 4)
        {
            printf("[ERROR] Unsupported channel number [%d] for [%s]\n", channels, filename);
            exit(-1);
        }

        m_width = width;
        m_height = height;
        m_channels = channels;
        if (m_data) delete[] m_data;
        m_data = new float[width * height * channels];
        memcpy(m_data, data, width * height * channels * sizeof(float));

        stbi_image_free(data);
    }

    /**
     * Save image in hdr format
     */
    void save(const char * filename)
    {
        if (ends_with(filename, ".jpg"))
        {
            stbi_uc * data = image_get_uchar_data();
            stbi_write_jpg(filename, m_width, m_height, m_channels, data, DEFAULT_JPEG_QUALITY);
            stbi_image_free(data);
        }
        else if (ends_with(filename, ".bmp"))
        {
            stbi_uc * data = image_get_uchar_data();
            stbi_write_bmp(filename, m_width, m_height, m_channels, data);
            stbi_image_free(data);
        }
        else if (ends_with(filename, ".hdr"))
        {
            stbi_write_hdr(filename, m_width, m_height, m_channels, m_data);
        }
        else if (ends_with(filename, ".png"))
        {
            stbi_uc * data = image_get_uchar_data();
            stbi_write_png(filename, m_width, m_height, m_channels, data, m_width * m_channels);
            stbi_image_free(data);
        }
        else if (ends_with(filename, ".tga"))
        {
            stbi_uc * data = image_get_uchar_data();
            stbi_write_tga(filename, m_width, m_height, m_channels, data);
            stbi_image_free(data);
        }
        else
        {
            printf("[ERROR] Unsupported image output format [%s]\n", filename);
            exit(-1);
        }
    }

    /**
     * Access to image data at certain integer position
     * (Deprecate)
     */
    float & pixel_at(int x, int y, int c)
    {
        int pos = (x + y * m_width) * m_channels + c;
        return m_data[pos];
    }

    /**
     * Access to image data at certain integer position
     * (Deprecate)
     */
    vec4 pixel_at(int x, int y) const
    {
        int pos = (x + y * m_width) * m_channels;
        if (m_channels == 3)
        {
            return vec4(
                m_data[pos],
                m_data[pos + 1],
                m_data[pos + 2],
                1.0f );
        }
        else
        {
            return vec4(
                m_data[pos],
                m_data[pos + 1],
                m_data[pos + 2],
                m_data[pos + 3] );
        }
    }

    /**
     * Sample a rgba color with a float uv coords
     * using bilinear interpolation
     */
    vec4 sample(const vec2 & uv) const
    {
        float fx = clamp(uv.x, 0.0f, 1.0f) * m_width - 0.5f;
        float fy = clamp(uv.y, 0.0f, 1.0f) * m_height - 0.5f;
        int x0 = static_cast<int>(floor(fx));
        int y0 = static_cast<int>(floor(fy));
        float alpha_x = fx - (float)x0;
        float alpha_y = fy - (float)y0;

        int x1 = x0 + 1;
        int y1 = y0 + 1;

        if (x0 < 0) x0 = 0;
        if (y0 < 0) y0 = 0;
        if (x0 == m_width - 1) x1 = x0;
        if (y0 == m_height - 1) y1 = y0;

        return  lerp( lerp(pixel_at(x0, y0), pixel_at(x1, y0), alpha_x),
                      lerp(pixel_at(x0, y1), pixel_at(x1, y1), alpha_x),
                      alpha_y );
    }

    int width() const { return m_width; }
    int height() const { return m_height; }
    int channels() const { return m_channels; }
    int size() const { return m_width * m_height * m_channels; }

    float * data() const { return m_data; }
};


}

#endif