#pragma once

#include "utils.hpp"
#include "color.hpp"

#include <cstdint>
#include <vector>
#include <string>

namespace cgfs
{

class Canvas final
{
public:

    explicit Canvas(const Dims dimensions, std::string name = "canvas", const Color& clearColor = Color::kBlack)
        : m_dimensions{ dimensions }
        , m_name{ std::move(name) }
    {
        assert(m_dimensions.is_valid());
        m_pixel_buffer.resize(m_dimensions.width * m_dimensions.height, Color::to_rgba_u8(clearColor));
    }

    // Canvas origin (0,0) is at the center.
    // x = [-canvas.w/2, canvas.w/2]
    // y = [-canvas.h/2, canvas.h/2]
    auto draw_pixel(const Point2 point, const Color& color) -> void
    {
        // Map back to "screen" coords with origin at the top-left corner.
        const auto x = ((m_dimensions.width  / 2) + point.x);
        const auto y = ((m_dimensions.height / 2) - point.y - 1);

        if (x < 0 || x >= m_dimensions.width ||
            y < 0 || y >= m_dimensions.height) [[unlikely]]
        {
            return;
        }

        const std::size_t pixel_idx = x + (y * m_dimensions.width);
        assert(pixel_idx < m_pixel_buffer.size());

        m_pixel_buffer[pixel_idx] = Color::to_rgba_u8(color);
    }

    // "Present" the canvas into a PNG image file.
    auto present() const -> bool;

    // Set whole canvas to the given pixel color.
    auto clear(const Color& clearColor = Color::kBlack) -> void;

    // Converts 2D canvas coordinates to 3D viewport coordinates.
    auto to_viewport(const Point2 point,
                     const float viewport_size = 1.0f,
                     const float projection_plane_z = 1.0f) const -> Vec3
    {
        return {
            static_cast<float>(point.x) * viewport_size / static_cast<float>(m_dimensions.width),
            static_cast<float>(point.y) * viewport_size / static_cast<float>(m_dimensions.height),
            projection_plane_z
        };
    }

    // Converts 2D viewport coordinates to 2D canvas coordinates (reverse of to_viewport).
    auto viewport_to_canvas(const float x, const float y,
                            const float viewport_size = 1.0f) const -> Point2
    {
        return {
            static_cast<int>(x * static_cast<float>(m_dimensions.width) / viewport_size),
            static_cast<int>(y * static_cast<float>(m_dimensions.height) / viewport_size)
        };
    }

    auto project_vertex(const Point3& v,
                        const float projection_plane_z = 1.0f) const -> Point2
    {
        return viewport_to_canvas(
            v.x * projection_plane_z / v.z,
            v.y * projection_plane_z / v.z);
    };

    auto unproject_vertex(const Point2 point, const float inv_z,
                          const float viewport_size = 1.0f,
                          const float projection_plane_z = 1.0f) const -> Point3
    {
        const float oz = 1.0f / inv_z;
        const float ux = point.x * oz / projection_plane_z;
        const float uy = point.y * oz / projection_plane_z;
        const auto p2d = to_viewport({ static_cast<int>(ux), static_cast<int>(uy) }, viewport_size, projection_plane_z);
        return { p2d.x, p2d.y, oz };
    }

    auto width() const -> int { return m_dimensions.width; }
    auto height() const -> int { return m_dimensions.height; }
    auto dimensions() const -> Dims { return m_dimensions; }
    auto name() const -> const std::string& { return m_name; }

    // No copy.
    Canvas(const Canvas& other) = delete;
    Canvas& operator=(const Canvas& other) = delete;

private:

    std::vector<RGBA_U8> m_pixel_buffer{};
    const Dims m_dimensions{};
    const std::string m_name{};
};

} // cgfs
