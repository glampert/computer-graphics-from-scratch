#pragma once

#include "vec4.hpp"
#include "mat3.hpp"

namespace cgfs
{

// 4x4 matrix.
struct Mat4 final
{
    float m[4][4];
    
    static const Mat4 kIdentity;

    static auto scaling(const float scale) -> Mat4
    {
        return { scale, 0.0f,  0.0f,  0.0f,
                 0.0f,  scale, 0.0f,  0.0f,
                 0.0f,  0.0f,  scale, 0.0f,
                 0.0f,  0.0f,  0.0f,  1.0f };
    }

    static auto translation(const Vec3& t) -> Mat4
    {
        return { 1.0f, 0.0f, 0.0f, t.x,
                 0.0f, 1.0f, 0.0f, t.y,
                 0.0f, 0.0f, 1.0f, t.z,
                 0.0f, 0.0f, 0.0f, 1.0f };
    }

    static auto rotation(const Mat3& mat) -> Mat4
    {
        Mat4 result = kIdentity;
        for (auto i = 0; i < 3; ++i)
        {
            for (auto j = 0; j < 3; ++j)
            {
                result.m[i][j] = mat.m[i][j];
            }
        }
        return result;
    }

    static auto transposed(const Mat4& mat) -> Mat4
    {
        Mat4 result = {};
        for (auto i = 0; i < 4; ++i)
        {
            for (auto j = 0; j < 4; ++j)
            {
                result.m[i][j] = mat.m[j][i];
            }
        }
        return result;
    }
};

// Matrix and vector multiply.
inline auto operator*(const Mat4& mat, const Vec4& vec) -> Vec4
{
    Vec4 result = {};
    for (auto i = 0; i < 4; ++i)
    {
        for (auto j = 0; j < 4; ++j)
        {
            result[i] += vec[j] * mat.m[i][j];
        }
    }
    return result;
}

// Matrix multiply.
inline auto operator*(const Mat4& matA, const Mat4& matB) -> Mat4
{
    Mat4 result = {};
    for (auto i = 0; i < 4; ++i)
    {
        for (auto j = 0; j < 4; ++j)
        {
            for (auto k = 0; k < 4; ++k)
            {
                result.m[i][j] += matA.m[i][k] * matB.m[k][j];
            }
        }
    }
    return result;
}

} // cgfs
