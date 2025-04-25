#pragma once

#include "../common/utils.hpp"
#include "../common/color.hpp"
#include "../common/texcoords.hpp"

#include <string>
#include <vector>

namespace cgfs::rasterizer
{

class Texture final
{
public:
    
    enum class Filter : int
    {
        kNearest,
        kBilinear,
        kTrilinear
    };

    Texture() = default;

    Texture(const std::string& filename, const Filter filter)
    {
        [[maybe_unused]] const bool is_loaded = load_from_file(filename, filter);
        assert(is_loaded);
    }
    
    auto load_from_file(const std::string& filename, const Filter filter) -> bool;

    // Get color for a texel with filtering applied.
    auto sample_texel(const TexCoords tex_coords) const -> Color;

    // Sample pixel directly without applying any filtering.
    auto pixel_at(std::size_t x, std::size_t y) const -> Color
    {
        x = std::min<std::size_t>(x, m_dimensions.width  - 1);
        y = std::min<std::size_t>(y, m_dimensions.height - 1);
        
        const auto offset = x + (y * m_dimensions.width);
        assert(offset < m_pixels.size());

        const auto px = m_pixels[offset];
        return Color::from_rgba_u8(px);
    }

    auto is_valid() const -> bool { return !m_pixels.empty() && m_dimensions.is_valid(); }
    auto width() const -> int { return m_dimensions.width; }
    auto height() const -> int { return m_dimensions.height; }
    auto dimensions() const -> Dims { return m_dimensions; }
    auto filter() const -> Filter { return m_filter; }
    
    // No copy.
    Texture(const Texture& other) = delete;
    Texture& operator=(const Texture& other) = delete;

    // Dummy empty texture - no texture assigned.
    static const Texture kNone;

private:
    
    Dims m_dimensions{};
    Filter m_filter{};
    std::vector<RGBA_U8> m_pixels{};
};

} // cgfs::rasterizer
