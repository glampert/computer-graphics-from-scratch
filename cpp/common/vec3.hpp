#pragma once

#include "utils.hpp"

namespace cgfs
{

struct Vec3 final
{
    float x{ 0.0f };
    float y{ 0.0f };
    float z{ 0.0f };

    constexpr Vec3() = default;
    
    constexpr Vec3(const float inX,
                   const float inY,
                   const float inZ)
        : x{ inX }
        , y{ inY }
        , z{ inZ }
    {}
    
    auto operator[](const std::size_t index) const -> const float
    {
        assert(index <= 2);
        return reinterpret_cast<const float*>(this)[index];
    }
    
    auto operator[](const std::size_t index) -> float&
    {
        assert(index <= 2);
        return reinterpret_cast<float*>(this)[index];
    }
};

// Reuse Vec3 as point for simplicity.
using Point3 = Vec3;

inline auto operator+(const Vec3& lhs, const Vec3& rhs) -> Vec3
{
    return { lhs.x + rhs.x,
             lhs.y + rhs.y,
             lhs.z + rhs.z };
}

inline auto operator-(const Vec3& lhs, const Vec3& rhs) -> Vec3
{
    return { lhs.x - rhs.x,
             lhs.y - rhs.y,
             lhs.z - rhs.z };
}

inline auto operator*(const Vec3& lhs, const Vec3& rhs) -> Vec3
{
    return { lhs.x * rhs.x,
             lhs.y * rhs.y,
             lhs.z * rhs.z };
}

inline auto operator*(const Vec3& v, const float amount) -> Vec3
{
    return { v.x * amount,
             v.y * amount,
             v.z * amount };
}

inline auto operator/(const Vec3& v, const float amount) -> Vec3
{
    return { v.x / amount,
             v.y / amount,
             v.z / amount };
}

inline auto operator-(const Vec3& v) -> Vec3
{
    return { -v.x, -v.y, -v.z };
}

inline auto operator+=(Vec3& lhs, const Vec3& rhs) -> Vec3&
{
    lhs = lhs + rhs;
    return lhs;
}

inline auto operator-=(Vec3& lhs, const Vec3& rhs) -> Vec3&
{
    lhs = lhs - rhs;
    return lhs;
}

inline auto operator*=(Vec3& v, const float amount) -> Vec3&
{
    v = v * amount;
    return v;
}

inline auto operator/=(Vec3& v, const float amount) -> Vec3&
{
    v = v / amount;
    return v;
}

inline auto dot(const Vec3& lhs, const Vec3& rhs) -> float
{
    return (lhs.x * rhs.x) +
           (lhs.y * rhs.y) +
           (lhs.z * rhs.z);
}

inline auto cross(const Vec3& lhs, const Vec3& rhs) -> Vec3
{
    return {
        (lhs.y * rhs.z) - (lhs.z * rhs.y),
        (lhs.z * rhs.x) - (lhs.x * rhs.z),
        (lhs.x * rhs.y) - (lhs.y * rhs.x) };
}

inline auto length(const Vec3& v) -> float
{
    return std::sqrt(dot(v, v));
}

inline auto length_squared(const Vec3& v) -> float
{
    return dot(v, v);
}

inline auto normalize(const Vec3& v) -> Vec3
{
    const float inv_len = 1.0f / length(v);
    return v * inv_len;
}

inline auto is_normalized(const Vec3& v, const float tolerance = 1e-6f) -> bool
{
    const float len = length(v);
    return std::fabs(len - 1.0f) < tolerance;
}

inline auto is_zero(const Vec3& v) -> bool
{
    return (v.x == 0.0f && v.y == 0.0f && v.z == 0.0f);
}

} // cgfs
