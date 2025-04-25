#pragma once

#include "mesh.hpp"
#include "scene.hpp"
#include "depth_buffer.hpp"
#include "../common/canvas.hpp"

namespace cgfs::rasterizer
{

struct DrawFlags final
{
    using Type = unsigned int;
    enum : Type
    {
        kWireframe          = 1 << 1,
        kColorFilled        = 1 << 2,
        kTextureMapped      = 1 << 3,
        kOutlines           = 1 << 4,
        kDepthTest          = 1 << 5,
        kBackFaceCull       = 1 << 6,
        kClipping           = 1 << 7,
        kComputeFaceNormals = 1 << 8 // Override model normals with a computed 'flat' face normal.
    };
};

struct LightModel final
{
    using Type = unsigned int;
    enum : Type
    {
        kDisabled = 0, // No lighting approximation. Triangle colors only.
        kDiffuse  = 1 << 1,
        kSpecular = 1 << 2
    };
};

enum class ShadeModel : int
{
    kDisabled, // No shading. Triangle colors only.
    kFlat,
    kGouraud,
    kPhong
};

struct DrawMeshParams final
{
    const Mesh& mesh;
    const Camera& camera;
    std::span<const Light> lights;

    DrawFlags::Type draw_flags;
    LightModel::Type light_model;
    ShadeModel shade_model;

    const Mat4& model_view_mtx;
    const Mat3& rotation;
    float scaling;
};

auto draw_mesh(Canvas& canvas,
               DepthBuffer& depth_buffer,
               const DrawMeshParams& params) -> void;

auto draw_scene(Canvas& canvas,
                DepthBuffer& depth_buffer,
                const Scene& scene,
                const DrawFlags::Type draw_flags,
                const LightModel::Type light_model,
                const ShadeModel shade_model) -> void;

} // cgfs::rasterizer
