#include "rects.hpp"
#include "lines.hpp"

namespace cgfs::rasterizer
{

auto draw_wireframe_rect(Canvas& canvas,
                         const Point2 top_left,
                         const Point2 bottom_left,
                         const Point2 top_right,
                         const Point2 bottom_right,
                         const Color& color) -> void
{
    draw_line(canvas, bottom_left,  top_left,     color);
    draw_line(canvas, top_left,     top_right,    color);
    draw_line(canvas, top_right,    bottom_right, color);
    draw_line(canvas, bottom_right, bottom_left,  color);
}

auto draw_filled_rect(Canvas& canvas,
                      const Point2 top_left,
                      const Point2 bottom_left,
                      const Point2 top_right,
                      const Point2 bottom_right,
                      const Color& color) -> void
{
    assert(bottom_left.y < top_left.y);
    assert(bottom_right.y < top_right.y);

    assert(top_left.x < top_right.x);
    assert(bottom_left.x < bottom_right.x);

    (void)top_right; // Not required.
    
    for (int y = bottom_left.y; y <= top_left.y; ++y)
    {
        for (int x = bottom_left.x; x <= bottom_right.x; ++x)
        {
            canvas.draw_pixel({ x, y }, color);
        }
    }
}

auto draw_shaded_rect(Canvas& canvas,
                      const Point2 top_left,
                      const float top_left_intensity,
                      const Point2 bottom_left,
                      const float bottom_left_intensity,
                      const Point2 top_right,
                      const float top_right_intensity,
                      const Point2 bottom_right,
                      const float bottom_right_intensity,
                      const Color& color) -> void
{
    assert(is_normalized(top_left_intensity));
    assert(is_normalized(bottom_left_intensity));
    assert(is_normalized(top_right_intensity));
    assert(is_normalized(bottom_right_intensity));

    assert(bottom_left.y < top_left.y);
    assert(bottom_right.y < top_right.y);

    assert(top_left.x < top_right.x);
    assert(bottom_left.x < bottom_right.x);
    
    (void)top_right; // Not required.

    const int height = (top_left.y - bottom_left.y);
    const int width  = (bottom_right.x - bottom_left.x);

    for (int y = bottom_left.y; y <= top_left.y; ++y)
    {
        const float delta_y = static_cast<float>(top_left.y - y) / static_cast<float>(height);
        
        for (int x = bottom_left.x; x <= bottom_right.x; ++x)
        {
            const float delta_x = static_cast<float>(bottom_right.x - x) / static_cast<float>(width);

            // Bilinear interpolation:
            const float top_intensity    = lerp(top_right_intensity, top_left_intensity, delta_x);
            const float bottom_intensity = lerp(bottom_right_intensity, bottom_left_intensity, delta_x);
            const float final_intensity  = lerp(top_intensity, bottom_intensity, delta_y);
            
            canvas.draw_pixel({ x, y }, color * final_intensity);
        }
    }
}

} // cgfs::rasterizer
