#pragma once

#include "vec3.hpp"

namespace cgfs
{

struct Vec4 final
{
    float x{ 0.0f };
    float y{ 0.0f };
    float z{ 0.0f };
    float w{ 0.0f };

    constexpr Vec4() = default;
    
    constexpr Vec4(const float inX,
                   const float inY,
                   const float inZ,
                   const float inW)
        : x{ inX }
        , y{ inY }
        , z{ inZ }
        , w{ inW }
    {}

    constexpr Vec4(const Vec3& xyz, const float inW)
        : x{ xyz.x }
        , y{ xyz.y }
        , z{ xyz.z }
        , w{ inW }
    {}

    auto operator[](const std::size_t index) const -> const float
    {
        assert(index <= 3);
        return reinterpret_cast<const float*>(this)[index];
    }
    
    auto operator[](const std::size_t index) -> float&
    {
        assert(index <= 3);
        return reinterpret_cast<float*>(this)[index];
    }
    
    // To Vec3.
    auto xyz() const -> Vec3 { return { x, y, z }; }
};

inline auto operator+(const Vec4& lhs, const Vec4& rhs) -> Vec4
{
    return { lhs.x + rhs.x,
             lhs.y + rhs.y,
             lhs.z + rhs.z,
             lhs.w + rhs.w };
}

inline auto operator-(const Vec4& lhs, const Vec4& rhs) -> Vec4
{
    return { lhs.x - rhs.x,
             lhs.y - rhs.y,
             lhs.z - rhs.z,
             lhs.w - rhs.w };
}

inline auto operator*(const Vec4& lhs, const Vec4& rhs) -> Vec4
{
    return { lhs.x * rhs.x,
             lhs.y * rhs.y,
             lhs.z * rhs.z,
             lhs.w * rhs.w };
}

inline auto operator*(const Vec4& v, const float amount) -> Vec4
{
    return { v.x * amount,
             v.y * amount,
             v.z * amount,
             v.w * amount };
}

inline auto operator/(const Vec4& v, const float amount) -> Vec4
{
    return { v.x / amount,
             v.y / amount,
             v.z / amount,
             v.w / amount };
}

inline auto operator-(const Vec4& v) -> Vec4
{
    return { -v.x, -v.y, -v.z, -v.w };
}

inline auto operator+=(Vec4& lhs, const Vec4& rhs) -> Vec4&
{
    lhs = lhs + rhs;
    return lhs;
}

inline auto operator-=(Vec4& lhs, const Vec4& rhs) -> Vec4&
{
    lhs = lhs - rhs;
    return lhs;
}

inline auto operator*=(Vec4& v, const float amount) -> Vec4&
{
    v = v * amount;
    return v;
}

inline auto operator/=(Vec4& v, const float amount) -> Vec4&
{
    v = v / amount;
    return v;
}

inline auto dot(const Vec4& lhs, const Vec4& rhs) -> float
{
    return (lhs.x * rhs.x) +
           (lhs.y * rhs.y) +
           (lhs.z * rhs.z) +
           (lhs.w * rhs.w);
}

inline auto length(const Vec4& v) -> float
{
    return std::sqrt(dot(v, v));
}

inline auto length_squared(const Vec4& v) -> float
{
    return dot(v, v);
}

inline auto normalize(const Vec4& v) -> Vec4
{
    const float inv_len = 1.0f / length(v);
    return v * inv_len;
}

inline auto is_normalized(const Vec4& v, const float tolerance = 1e-6f) -> bool
{
    const float len = length(v);
    return std::fabs(len - 1.0f) < tolerance;
}

inline auto is_zero(const Vec4& v) -> bool
{
    return (v.x == 0.0f && v.y == 0.0f && v.z == 0.0f && v.w == 0.0f);
}

} // cgfs
