#pragma once

namespace cgfs
{

struct TexCoords final
{
    float u{ 0.0f };
    float v{ 0.0f };
};

inline auto operator+(const TexCoords& lhs, const TexCoords& rhs) -> TexCoords
{
    return { lhs.u + rhs.u, lhs.v + rhs.v };
}

inline auto operator-(const TexCoords& lhs, const TexCoords& rhs) -> TexCoords
{
    return { lhs.u - rhs.u, lhs.v - rhs.v };
}

inline auto operator/(const TexCoords& tc, const float amount) -> TexCoords
{
    return { tc.u / amount, tc.v / amount };
}

inline auto operator*(const TexCoords& tc, const float amount) -> TexCoords
{
    return { tc.u * amount, tc.v * amount };
}

inline auto operator+=(TexCoords& lhs, const TexCoords& rhs) -> TexCoords&
{
    lhs = lhs + rhs;
    return lhs;
}

inline auto operator-=(TexCoords& lhs, const TexCoords& rhs) -> TexCoords&
{
    lhs = lhs - rhs;
    return lhs;
}

inline auto operator/=(TexCoords& tc, const float amount) -> TexCoords&
{
    tc = tc / amount;
    return tc;
}

inline auto operator*=(TexCoords& tc, const float amount) -> TexCoords&
{
    tc = tc * amount;
    return tc;
}

} // cgfs
