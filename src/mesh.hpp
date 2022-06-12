#ifndef __MESH_HPP__
#define __MESH_HPP__

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "thirdparty/tinyobjloader/tiny_obj_loader.h"
#include "global.hpp"

namespace Utility
{

struct Vertex
{
    vec3 position;
    vec3 normal;
    vec2 texcoord;
};

class Mesh
{
private:
    std::vector<Vertex> m_vertices;
    std::vector<size_t> m_indices;

public:
    Mesh(const char * filename)
    {
        std::string inputfile(filename);
        tinyobj::ObjReaderConfig reader_config;
        reader_config.mtl_search_path = "./";

        tinyobj::ObjReader reader;
        if (!reader.ParseFromFile(inputfile, reader_config))
        {
            if (!reader.Error().empty())
            {
                printf("[ERROR] Tinyobjloader: %s\n", reader.Error().c_str());
            }
            exit(-1);
        }

        if (!reader.Warning().empty()) 
        {
            printf("[WARNING] Tinyobjloader: %s\n", reader.Warning().c_str());
        }

        const tinyobj::attrib_t & attrib = reader.GetAttrib();
        const std::vector<tinyobj::shape_t> & shapes = reader.GetShapes();

        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[0].mesh.num_face_vertices.size(); f++)
        {
            size_t fv = size_t(shapes[0].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++)
            {
                // access to vertex
                tinyobj::index_t idx = shapes[0].mesh.indices[index_offset + v];
                m_indices.push_back(index_offset + v);
                Vertex vertex;

                tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
                vertex.position = vec3(vx, vy, vz);

                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0)
                {
                    tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                    tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                    tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
                    vertex.normal = vec3(nx, ny, nz);
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0)
                {
                    tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                    tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
                    vertex.texcoord = vec2(tx, ty);
                }
                m_vertices.push_back(vertex);
            }
            index_offset += fv;
        }
    }

    const std::vector<Vertex> & vertices() const { return m_vertices; }
    const std::vector<size_t> & indices() const { return m_indices; }
};

}

#endif