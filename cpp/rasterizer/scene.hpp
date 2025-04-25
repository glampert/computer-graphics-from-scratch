#pragma once

#include "../common/plane.hpp"
#include "mesh.hpp"

#include <span>

namespace cgfs::rasterizer
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
};

struct Camera final
{
    Point3 position{};
    Mat3 rotation{ Mat3::kIdentity };
    ClippingPlanes clipping_planes{};

    auto to_mat4() const -> Mat4
    {
        // Camera Mtx = C.r−1 * C.t−1
        const Mat4 r = Mat4::transposed(Mat4::rotation(rotation));
        const Mat4 t = Mat4::translation(-position);
        return r * t;
    }
};

struct Scene final
{
    const Camera& camera{};
    std::span<const Mesh::Instance> meshes_instances{};
    std::span<const Light> lights{};
};

} // cgfs::rasterizer
