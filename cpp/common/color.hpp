#pragma once

#include "vec3.hpp"
#include <cstdint>
#include <algorithm>

namespace cgfs
{

struct RGBA_U8 final
{
    std::uint8_t r{};
    std::uint8_t g{};
    std::uint8_t b{};
    std::uint8_t a{};
};

struct Color final
{
    float r{ 0.0f };
    float g{ 0.0f };
    float b{ 0.0f };
    float a{ 0.0f };
    
    constexpr Color() = default;
    
    constexpr Color(const float inR,
                    const float inG,
                    const float inB,
                    const float inA = 1.0f)
        : r{ inR }
        , g{ inG }
        , b{ inB }
        , a{ inA }
    {}

    static const Color kBlack;
    static const Color kWhite;
    static const Color kGray;
    static const Color kBrightRed;
    static const Color kBrightGreen;
    static const Color kBrightBlue;
    static const Color kBrightYellow;
    static const Color kDarkRed;
    static const Color kDarkGreen;
    static const Color kDarkBlue;
    static const Color kDarkYellow;
    static const Color kCyan;
    static const Color kPurple;

    static auto to_vec3(const Color& c) -> Vec3
    {
        return { c.r, c.g, c.b };
    }

    static auto from_vec3(const Vec3& v, const float alpha = 1.0f) -> Color
    {
        return { v.x, v.y, v.z, alpha };
    }

    static auto to_rgba_u8(const Color& c) -> RGBA_U8
    {
        assert(is_normalized(c.r) &&
               is_normalized(c.g) &&
               is_normalized(c.b) &&
               is_normalized(c.a));
        
        return {
            static_cast<std::uint8_t>(c.r * 255.0f),
            static_cast<std::uint8_t>(c.g * 255.0f),
            static_cast<std::uint8_t>(c.b * 255.0f),
            static_cast<std::uint8_t>(c.a * 255.0f)
        };
    }

    static auto from_rgba_u8(const RGBA_U8 u) -> Color
    {
        return {
            static_cast<float>(u.r) / 255.0f,
            static_cast<float>(u.g) / 255.0f,
            static_cast<float>(u.b) / 255.0f,
            static_cast<float>(u.a) / 255.0f
        };
    }
};

inline auto operator*(const Color& lhs, const Color& rhs) -> Color
{
    return {
        lhs.r * rhs.r,
        lhs.g * rhs.g,
        lhs.b * rhs.b,
        lhs.a * rhs.a };
}

inline auto operator*(const Color& c, const float amount) -> Color
{
    return {
        c.r * amount,
        c.g * amount,
        c.b * amount,
        c.a * amount };
}

inline auto operator/(const Color& c, const float amount) -> Color
{
    return {
        c.r / amount,
        c.g / amount,
        c.b / amount,
        c.a / amount };
}

inline auto operator+(const Color& lhs, const Color& rhs) -> Color
{
    return {
        lhs.r + rhs.r,
        lhs.g + rhs.g,
        lhs.b + rhs.b,
        lhs.a + rhs.a };
}

inline auto operator-(const Color& lhs, const Color& rhs) -> Color
{
    return {
        lhs.r - rhs.r,
        lhs.g - rhs.g,
        lhs.b - rhs.b,
        lhs.a - rhs.a };
}

inline auto operator*=(Color& lhs, const Color& rhs) -> Color&
{
    lhs = lhs * rhs;
    return lhs;
}

inline auto operator*=(Color& c, const float amount) -> Color&
{
    c = c * amount;
    return c;
}

inline auto operator/=(Color& c, const float amount) -> Color&
{
    c = c / amount;
    return c;
}

inline auto operator+=(Color& lhs, const Color& rhs) -> Color&
{
    lhs = lhs + rhs;
    return lhs;
}

inline auto operator-=(Color& lhs, const Color& rhs) -> Color&
{
    lhs = lhs - rhs;
    return lhs;
}

inline auto is_normalized(const Color& c) -> bool
{
    return is_normalized(c.r) &&
           is_normalized(c.g) &&
           is_normalized(c.b) &&
           is_normalized(c.a);
}

inline auto is_zero(const Color& c) -> bool
{
    return (c.r == 0.0f && c.g == 0.0f && c.b == 0.0f && c.a == 0.0f);
}

} // cgfs
