#pragma once

#include "vec3.hpp"
#include <array>

namespace cgfs
{

struct Plane final
{
    Vec3 normal{};
    float distance{ 0.0f };
};

inline auto signed_distance(const Plane& plane, const Vec3& vertex) -> float
{
    return dot(plane.normal, vertex) + plane.distance;
}

struct ClippingPlanes final
{
    enum PlaneIdx : unsigned int
    {
        kNear,
        kLeft,
        kRight,
        kTop,
        kBottom,

        kCount = 5
    };

    // Near, Left, Right, Top, Bottom.
    std::array<Plane, kCount> planes{};
};

} // cgfs
