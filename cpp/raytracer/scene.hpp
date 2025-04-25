#pragma once

#include "../common/vec3.hpp"
#include "../common/mat3.hpp"
#include "../common/color.hpp"

#include <vector>
#include <span>

namespace cgfs::raytracer
{

struct Light final
{
    enum class Type : int
    {
        kAmbient,
        kDirectional,
        kPoint
    };

    Type type{};
    Point3 position{};
    float intensity{ 0.0f };
    Color color{ Color::kWhite };
};

struct Material final
{
    Color color{};
    float specular{ 0.0f }; // 0=matte surface.
    float reflectiveness{ 0.0f }; // 0=not reflective, 1=perfect mirror.
    float refractive_index{ 0.0f }; // 1=fully transparent, higher the value the more opaque; 0=disables refraction.
};

struct Sphere final
{
    // Simple helper to store the squared radius of a sphere.
    struct Radius final
    {
        float squared{};
        
        constexpr Radius(const float radius)
            : squared(radius * radius)
        {}
    };
    
    Material material{};
    Point3 center{};
    Radius radius{ 0.0f };
};

struct Mesh final
{
    // For simplicity, one material per whole mesh for now.
    // Could be per sub-mesh or even per face.
    Material material{};

    struct Face final
    {
        // Indices making a triangle or "face".
        std::uint16_t verts[3]{};

        // Index of the face normal.
        std::uint16_t normal{};
    };

    std::vector<Point3> vertices{};
    std::vector<Vec3> normals{};
    std::vector<Face> faces{};
};

struct Camera final
{
    Point3 position{};
    Mat3 rotation{ Mat3::kIdentity };
};

struct Scene final
{
    std::span<const Sphere> spheres{};
    std::span<const Mesh> meshes{};
    std::span<const Light> lights{};
};

// Simple .obj 3D model loader.
auto load_obj_mesh_from_file(Mesh& mesh, const std::string& filename, const float vertex_scale = 1.0f) -> bool;

} // cgfs::raytracer
