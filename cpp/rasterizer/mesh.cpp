#include "mesh.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

namespace cgfs::rasterizer
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
        // Parse texture coordinates (vt)
        else if (prefix == "vt")
        {
            TexCoords tc = {};
            ss >> tc.u >> tc.v;
            // Flip uvs:
            tc.v = 1.0f - tc.v;
            mesh.tex_coords.push_back(tc);
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
                std::string face_tex_coords{};
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
                    face_tex_coords = face_info.substr(single_slash + 1, last_slash - (single_slash + 1));
                    face_normal = face_info.substr(last_slash + 1);
                }
                else [[unlikely]]
                {
                    std::println(stderr, "Unsupported OBJ model face format: `{}`", face_info);
                    return false;
                }

                const int vertex_index = std::stoi(face_vertex) - 1; // Convert from 1-based to 0-based.
                assert(vertex_index >= 0 && vertex_index <= UINT16_MAX); // 16bit indices.

                // Optional tex coords.
                const int tex_coord_index = !face_tex_coords.empty() ? (std::stoi(face_tex_coords) - 1) : 0;
                assert(tex_coord_index >= 0 && tex_coord_index <= UINT16_MAX);
                
                const int normal_index = std::stoi(face_normal) - 1;
                assert(normal_index >= 0 && normal_index <= UINT16_MAX);

                face.verts[count] = static_cast<std::uint16_t>(vertex_index);
                face.tex_coords[count] = static_cast<std::uint16_t>(tex_coord_index);
                face.normals[count] = static_cast<std::uint16_t>(normal_index);
            }

            mesh.faces.push_back(face);
        }
    }

    return true;
}

auto compute_bounding_sphere(const std::vector<Point3>& points) -> Mesh::BoundingSphere
{
    if (points.empty())
    {
        return {};
    }

    // Step 1: Find the most distant pair of points
    Point3 p0 = points[0];
    Point3 p1 = points[0];
    float maxDistSq = 0.0f;

    for (const auto& a : points)
    {
        for (const auto& b : points)
        {
            const Point3 d = b - a;
            const float distSq = dot(d, d);
            if (distSq > maxDistSq)
            {
                maxDistSq = distSq;
                p0 = a;
                p1 = b;
            }
        }
    }

    // Step 2: Initial sphere center = midpoint
    Point3 center = (p0 + p1) / 2.0f;
    float radius = std::sqrt(maxDistSq) / 2.0f;

    // Step 3: Grow sphere to include all points
    for (const auto& p : points)
    {
        const Point3 d = p - center;
        const float distSq = dot(d, d);

        if (distSq > (radius * radius))
        {
            const float dist = std::sqrt(distSq);
            const float newRadius = (radius + dist) / 2.0f;

            // Move center toward the point
            const float k = (newRadius - radius) / dist;
            center.x += d.x * k;
            center.y += d.y * k;
            center.z += d.z * k;
            radius = newRadius;
        }
    }

    return { center, radius };
}

} // cgfs::rasterizer
