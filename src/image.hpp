#ifndef __IMAGE_HPP__
#define __IMAGE_HPP__

#include <stdlib.h>
#include <string.h>
#define STB_IMAGE_IMPLEMENTATION
#include "thirdparty/stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "thirdparty/stb/stb_image_write.h"
#include "global.hpp"
#ifdef _OPENMP
#include <omp.h>
#endif

using std::make_shared;
using std::shared_ptr;

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
     * Save image
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

    float & operator () (int x, int y, int c)
    {
        int pos = (x + (m_height - 1 - y) * m_width) * m_channels + c;
        return m_data[pos];
    }

    /**
     * Access to image data at certain integer position
     * (Deprecate)
     */
    float pixel_at(int x, int y, int c) const
    {
        int pos = (x + (m_height - 1 - y) * m_width) * m_channels + c;
        return m_data[pos];
    }

    /**
     * Access to image data at certain integer position
     * (Deprecate)
     */
    vec4 pixel_at(int x, int y) const
    {
        int pos = (x + (m_height - 1 - y) * m_width) * m_channels;
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
        float fx = CLAMP(uv.x, 0.0f, 1.0f) * m_width - 0.5f;
        float fy = CLAMP(uv.y, 0.0f, 1.0f) * m_height - 0.5f;
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

        return  LERP( LERP(pixel_at(x0, y0), pixel_at(x1, y0), alpha_x),
                      LERP(pixel_at(x0, y1), pixel_at(x1, y1), alpha_x),
                      alpha_y );
    }

    int width() const { return m_width; }
    int height() const { return m_height; }
    int channels() const { return m_channels; }
    int size() const { return m_width * m_height * m_channels; }

    float * data() const { return m_data; }
};

float gaussian(float value, float sigma)
{
    return expf(-fabs(value) / (2.0f * sigma * sigma));
}

std::vector<float> generate_gaussian_kernel(int size, float sigma)
{
    int half_size = abs(size) / 2;
    int k_index = 0;
    std::vector<float> kernel;
    kernel.reserve(size * size);

    for (int x = -half_size; x <= half_size; x++)
    {
        for (int y = -half_size; y <= half_size; y++)
        {
            kernel[k_index] = gaussian(x * x + y * y, sigma);
            k_index++;
        }
    }

    return kernel;
}

Image bilateral_filtering(const Image & image, int kernel_size, float sigma_r, float sigma_s)
{
    assert(kernel_size % 2 == 1);

    auto space_kernel = generate_gaussian_kernel(kernel_size, sigma_s);

    int half_size = kernel_size / 2;
    clock_t start_timestamp = clock();
    Image result(image.width(), image.height(), image.channels());

#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (int x = 0; x < image.width(); x++)
    for (int y = 0; y < image.height(); y++)
    {
        for (int c = 0; c < image.channels(); c++)
        {
            float weight = 0.0f;
            float value = 0.0f;
            float factor = 0.0f;
            int k_index = 0;
            for (int k_x = x - half_size; k_x <= x + half_size; k_x++)
            for (int k_y = y - half_size; k_y <= y + half_size; k_y++)
            {
                int s_x = CLAMP(k_x, 0, image.width() - 1);
                int s_y = CLAMP(k_y, 0, image.height() - 1);
                factor = gaussian(image.pixel_at(s_x, s_y, c) - image.pixel_at(x, y, c), sigma_r)
                       * space_kernel[k_index];
                k_index++;
                weight += factor;
                value += factor * image.pixel_at(s_x, s_y, c);
            }
            result(x, y, c) = value / weight;
        }
    }

    char total_time[DURATION_STR_LENGTH];
    float duration = ((float)(clock() - start_timestamp) / CLOCKS_PER_SEC);
    get_duration_str(duration, total_time);
    printf("[INFO] Bilateral filtering time: %s\n", total_time);

    return result;
}

class Texture
{
public:
    virtual vec4 value(float u, float v, const vec3 & p) const = 0;
};

class SolidColor : public Texture
{
private:
    vec4 m_color;

public:
    SolidColor() = delete;

    SolidColor(vec4 color): m_color(color) {}

    SolidColor(float r, float g, float b):
        SolidColor(vec4(r, g, b, 1.0f)) {}

    virtual vec4 value(float u, float v, const vec3 & p) const override
    {
        return m_color;
    }
};

class CheckerTexture : public Texture
{
private:
    shared_ptr<Texture> m_odd;
    shared_ptr<Texture> m_even;
    int m_num;

public:
    CheckerTexture() = delete;

    CheckerTexture(shared_ptr<Texture> even, shared_ptr<Texture> odd, int num = 10): 
        m_even(even), m_odd(odd), m_num(num) {}

    CheckerTexture(vec4 color0, vec4 color1, int num = 10): 
        m_even(make_shared<SolidColor>(color0)), 
        m_odd(make_shared<SolidColor>(color1)),
        m_num(num) {}

    virtual vec4 value(float u, float v, const vec3 & p) const override
    {
        int x = u * m_num;
        int y = v * m_num;
        if ((x % 2 + y % 2) % 2 == 1)
            return m_odd->value(u, v, p);
        else
            return m_even->value(u, v, p);
        // float sines = sinf(10.0f * p.x) * sinf(10.0f * p.y) * sinf(10.0f * p.z);
        // if (sines < 0)
        //     return m_odd->value(u, v, p);
        // else
        //     return m_even->value(u, v, p);
    }


};

} // namespace Utility

#endif