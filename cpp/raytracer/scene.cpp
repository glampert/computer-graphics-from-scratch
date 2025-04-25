#include "scene.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

namespace cgfs::raytracer
{

auto load_obj_mesh_from_file(Mesh& mesh, const std::string& filename, const float vertex_scale) -> bool
{
    std::ifstream file{ filename };
    if (!file.is_open())
    {
        std::println(stderr, "Error: Could not open file: '{}'", filename);
        return false;
    }

    std::string line{};
    while (std::getline(file, line))
    {
        std::istringstream ss{ line };
        std::string prefix{};
        ss >> prefix;

        // Parse vertices (v)
        if (prefix == "v")
        {
            Point3 v = {};
            ss >> v.x >> v.y >> v.z;
            mesh.vertices.push_back(v * vertex_scale);
        }
        // Parse normals (vn)
        else if (prefix == "vn")
        {
            Vec3 vn = {};
            ss >> vn.x >> vn.y >> vn.z;
            mesh.normals.push_back(vn);
        }
        // Parse faces (f)
        else if (prefix == "f")
        {
            Mesh::Face face = {};
            std::string face_info{};

            // Only support triangles (3 vertices per face).
            for (int count = 0; ss >> face_info; ++count)
            {
                if (count >= 3) [[unlikely]]
                {
                    std::println(stderr, "Non-triangle OBJ model face: `{}`", face_info);
                    return false;
                }
                
                std::string face_vertex{};
                std::string face_normal{};

                // `f vertex_index//normal_index`
                if (const auto double_slash = face_info.find("//"); double_slash != std::string::npos)
                {
                    face_vertex = face_info.substr(0, double_slash);
                    face_normal = face_info.substr(double_slash + 2); // +2 skips '//' separator
                }
                // `f vertex_index/texture_index/normal_index`
                else if (const auto single_slash = face_info.find('/'); single_slash != std::string::npos)
                {
                    const auto last_slash = face_info.find_last_of('/');
                    face_vertex = face_info.substr(0, single_slash);
                    face_normal = face_info.substr(last_slash + 1);
                }
                else [[unlikely]]
                {
                    std::println(stderr, "Unsupported OBJ model face format: `{}`", face_info);
                    return false;
                }

                const int vertex_index = std::stoi(face_vertex) - 1; // Convert from 1-based to 0-based.
                assert(vertex_index >= 0 && vertex_index <= UINT16_MAX); // 16bit indices.

                const int normal_index = std::stoi(face_normal) - 1;
                assert(normal_index >= 0 && normal_index <= UINT16_MAX);

                face.verts[count] = static_cast<std::uint16_t>(vertex_index);
                face.normal = static_cast<std::uint16_t>(normal_index);
            }

            mesh.faces.push_back(face);
        }
    }

    return true;
}

} // cgfs::raytracer
