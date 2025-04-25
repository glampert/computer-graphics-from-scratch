#include "lines.hpp"

#include <algorithm>

namespace cgfs::rasterizer
{

static auto draw_line_horizontal(Canvas& canvas,
                                 Point2 p0,
                                 Point2 p1,
                                 const Color& color) -> void
{
    // Make sure x0 < x1
    if (p0.x > p1.x)
    {
        std::swap(p0, p1);
    }

    const auto a = static_cast<float>(p1.y - p0.y) / static_cast<float>(p1.x - p0.x);
    auto y = static_cast<float>(p0.y);
    
    for (int x = p0.x; x <= p1.x; ++x)
    {
        canvas.draw_pixel({ x, static_cast<int>(y) }, color);
        y = y + a;
    }
}

static auto draw_line_vertical(Canvas& canvas,
                               Point2 p0,
                               Point2 p1,
                               const Color& color) -> void
{
    // Make sure y0 < y1
    if (p0.y > p1.y)
    {
        std::swap(p0, p1);
    }

    const auto a = static_cast<float>(p1.x - p0.x) / static_cast<float>(p1.y - p0.y);
    auto x = static_cast<float>(p0.x);

    for (int y = p0.y; y <= p1.y; ++y)
    {
        canvas.draw_pixel({ static_cast<int>(x), y }, color);
        x = x + a;
    }
}

// Simple reference implementation of line drawing.
// Uses floating point operations but is the most straightforward.
static auto draw_line_generic_simple(Canvas& canvas,
                                     Point2 p0,
                                     Point2 p1,
                                     const Color& color) -> void
{
    const int dx = p1.x - p0.x;
    const int dy = p1.y - p0.y;

    if (std::abs(dx) > std::abs(dy))
    {
        // Line is horizontal-ish.
        draw_line_horizontal(canvas, p0, p1, color);
    }
    else
    {
        // Line is vertical-ish.
        draw_line_vertical(canvas, p0, p1, color);
    }
}

// Draws a line using Bresenham's algorithm, which only uses integer operations.
// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
static auto draw_line_bresenham(Canvas& canvas,
                                Point2 p0,
                                Point2 p1,
                                const Color& color) -> void
{
    int dx = std::abs(p1.x - p0.x);
    int dy = std::abs(p1.y - p0.y);

    const bool is_steep = dy > dx;
    if (is_steep)
    {
        std::swap(dx, dy);
    }

    const int sx = (p0.x < p1.x) ? 1 : -1;
    const int sy = (p0.y < p1.y) ? 1 : -1;

    int err = (2 * dy) - dx;
    int x = p0.x;
    int y = p0.y;

    for (int i = 0; i <= dx; ++i)
    {
        canvas.draw_pixel({ x, y }, color);

        while (err > 0)
        {
            if (is_steep)
            {
                x += sx;
            }
            else
            {
                y += sy;
            }
            err -= 2 * dx;
        }

        if (is_steep)
        {
            y += sy;
        }
        else
        {
            x += sx;
        }
        err += 2 * dy;
    }
}

auto draw_line(Canvas& canvas,
               const Point2 p0,
               const Point2 p1,
               const Color& color) -> void
{
    draw_line_bresenham(canvas, p0, p1, color);
}

} // cgfs::rasterizer
