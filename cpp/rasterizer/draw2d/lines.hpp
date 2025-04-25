#pragma once

#include "../../common/canvas.hpp"

namespace cgfs::rasterizer
{

auto draw_line(Canvas& canvas,
               const Point2 p0,
               const Point2 p1,
               const Color& color) -> void;

} // cgfs::rasterizer
