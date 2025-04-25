#pragma once

#include "vec3.hpp"
#include <cmath>

namespace cgfs
{

// 3x3 rotation matrix.
struct Mat3 final
{
    float m[3][3];
    
    static const Mat3 kIdentity;
    
    static auto rotation_x(const float degrees) -> Mat3
    {
        const float sin = std::sin(degrees_to_radians(degrees));
        const float cos = std::cos(degrees_to_radians(degrees));
        return { 1.0f,  0.0f, 0.0f,
                 0.0f,  cos,  sin,
                 0.0f, -sin,  cos };
    }

    static auto rotation_y(const float degrees) -> Mat3
    {
        const float sin = std::sin(degrees_to_radians(degrees));
        const float cos = std::cos(degrees_to_radians(degrees));
        return { cos,  0.0f, -sin,
                 0.0f, 1.0f,  0.0f,
                 sin,  0.0f,  cos };
    }
    
    static auto rotation_z(const float degrees) -> Mat3
    {
        const float sin = std::sin(degrees_to_radians(degrees));
        const float cos = std::cos(degrees_to_radians(degrees));
        return {  cos,  sin,  0.0f,
                 -sin,  cos,  0.0f,
                  0.0f, 0.0f, 1.0f };
    }

    static auto transposed(const Mat3& mat) -> Mat3
    {
        Mat3 result = {};
        for (auto i = 0; i < 3; ++i)
        {
            for (auto j = 0; j < 3; ++j)
            {
                result.m[i][j] = mat.m[j][i];
            }
        }
        return result;
    }
};

// Matrix and vector multiply.
inline auto operator*(const Mat3& mat, const Vec3& vec) -> Vec3
{
    Vec3 result = {};
    for (auto i = 0; i < 3; ++i)
    {
        for (auto j = 0; j < 3; ++j)
        {
            result[i] += vec[j] * mat.m[i][j];
        }
    }
    return result;
}

// Matrix multiply.
inline auto operator*(const Mat3& matA, const Mat3& matB) -> Mat3
{
    Mat3 result = {};
    for (auto i = 0; i < 3; ++i)
    {
        for (auto j = 0; j < 3; ++j)
        {
            for (auto k = 0; k < 3; ++k)
            {
                result.m[i][j] += matA.m[i][k] * matB.m[k][j];
            }
        }
    }
    return result;
}

} // cgfs
