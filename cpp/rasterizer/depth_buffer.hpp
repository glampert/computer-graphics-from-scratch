#pragma once

#include <vector>

namespace cgfs::rasterizer
{

class DepthBuffer final
{
public:
    
    explicit DepthBuffer(const Dims dimensions)
        : m_dimensions{ dimensions }
    {
        assert(m_dimensions.is_valid());
        m_buffer.resize(m_dimensions.width * m_dimensions.height, 0.0f);
    }

    // Origin (0,0) is at the center (same as the canvas).
    // x = [-buffer.w/2, buffer.w/2]
    // y = [-buffer.h/2, buffer.h/2]
    // Returns: true if current Z is lower and new value was written, false if existing Z is higher.
    auto test_and_set(const Point2 point, const float inv_z) -> bool
    {
        // Map back to "screen" coords with origin at the top-left corner.
        const auto x = ((m_dimensions.width  / 2) + point.x);
        const auto y = ((m_dimensions.height / 2) - point.y - 1);

        if (x < 0 || x >= m_dimensions.width ||
            y < 0 || y >= m_dimensions.height) [[unlikely]]
        {
            return false;
        }
        
        const std::size_t buffer_idx = x + (y * m_dimensions.width);
        assert(buffer_idx < m_buffer.size());

        if (m_buffer[buffer_idx] < inv_z)
        {
            m_buffer[buffer_idx] = inv_z;
            return true;
        }
        
        return false;
    }

    auto clear() -> void
    {
        std::fill(m_buffer.begin(), m_buffer.end(), 0.0f);
    }
    
    auto width() const -> int { return m_dimensions.width; }
    auto height() const -> int { return m_dimensions.height; }
    auto dimensions() const -> Dims { return m_dimensions; }

    // No copy.
    DepthBuffer(const DepthBuffer& other) = delete;
    DepthBuffer& operator=(const DepthBuffer& other) = delete;
    
private:
    
    const Dims m_dimensions{};
    std::vector<float> m_buffer{};
};

} // cgfs::rasterizer
