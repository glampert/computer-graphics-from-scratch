#pragma once

#include <cmath>
#include <cstddef>
#include <cassert>

namespace cgfs
{

constexpr float kInfinity = INFINITY;
constexpr float kPI = static_cast<float>(M_PI);

constexpr float degrees_to_radians(const float degrees)
{
    return degrees * (kPI / 180.0f);
}

constexpr float radians_to_degrees(const float radians)
{
    return radians * (180.0f / kPI);
}

inline auto is_zero(const float f) -> bool
{
    return (f == 0.0f);
}

inline auto is_normalized(const float f) -> bool
{
    return (f >= 0.0f && f <= 1.0f);
}

inline auto lerp(const float a, const float b, const float t) -> float
{
    return (1.0f - t) * a + t * b;
}

inline auto frac(const float x) -> float
{
    return x - std::floor(x);
}

struct Point2 final
{
    int x{ 0 };
    int y{ 0 };
};

struct Dims final
{
    int width{ 0 };
    int height{ 0 };
    
    auto is_valid() const -> bool
    {
        return width > 0 && height > 0;
    }
};

} // cfg
