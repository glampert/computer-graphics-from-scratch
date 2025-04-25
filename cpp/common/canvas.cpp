#include "canvas.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_STATIC
#include "../external/stb_image_write.h"

namespace cgfs
{

auto Canvas::present() const -> bool
{
    const int bytes_per_pixel = 4; // RGBA U8 Color
    const int stride_in_bytes = m_dimensions.width * bytes_per_pixel;
    
    const int result = stbi_write_png((m_name + ".png").c_str(), m_dimensions.width, m_dimensions.height,
                                      bytes_per_pixel, m_pixel_buffer.data(), stride_in_bytes);
    return result != 0;
}

auto Canvas::clear(const Color& clearColor) -> void
{
    std::fill(m_pixel_buffer.begin(), m_pixel_buffer.end(), Color::to_rgba_u8(clearColor));
}

} // cgfs
