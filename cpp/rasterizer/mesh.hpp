#pragma once

#include "../common/color.hpp"
#include "../common/vec3.hpp"
#include "../common/vec4.hpp"
#include "../common/mat3.hpp"
#include "../common/mat4.hpp"
#include "../common/texcoords.hpp"
#include "texture.hpp"

#include <string>
#include <vector>

namespace cgfs::rasterizer
{

struct Mesh final
{
    struct Transform final
    {
        Vec3 translation{ 0.0f, 0.0f, 0.0f };
        Mat3 rotation{ Mat3::kIdentity };
        float scaling{ 1.0f }; // Uniform scaling.

        auto to_mat4() const -> Mat4
        {
            // Model Mtx = T*R*S
            const Mat4 t = Mat4::translation(translation);
            const Mat4 r = Mat4::rotation(rotation);
            const Mat4 s = Mat4::scaling(scaling);
            return t * r * s;
        }
    };

    struct Instance final
    {
        const Mesh& mesh;
        Transform transform{};
    };
    
    struct BoundingSphere final
    {
        Point3 center{};
        float radius{ 0.0f };
    };

    struct Face final
    {
        // Indices making a triangle or "face".
        std::uint16_t verts[3]{};

        // Indices of the 3 face normals.
        std::uint16_t normals[3]{};

        // Indices of the 3 face UV sets.
        std::uint16_t tex_coords[3]{};

        // Face material.
        Color color{};
        float specular{ 0.0f }; // 0=matte surface.
        
        // Texture::kNone means no texture assigned. Face tex_coords are ignored.
        const Texture* texture{ &Texture::kNone };
    };

    std::vector<Point3> vertices{};
    std::vector<Vec3> normals{};
    std::vector<TexCoords> tex_coords{};
    std::vector<Face> faces{};

    // For clipping.
    BoundingSphere bounding_sphere{};
};

// Simple .obj 3D model loader.
auto load_obj_mesh_from_file(Mesh& mesh, const std::string& filename, const float vertex_scale = 1.0f) -> bool;

// Compute sphere bounds using Ritter's algorithm, simple but not necessarily the most accurate or fastest.
auto compute_bounding_sphere(const std::vector<Point3>& points) -> Mesh::BoundingSphere;

} // cgfs::rasterizer
