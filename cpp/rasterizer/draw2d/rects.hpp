#pragma once

#include "../../common/canvas.hpp"

namespace cgfs::rasterizer
{

auto draw_wireframe_rect(Canvas& canvas,
                         const Point2 top_left,
                         const Point2 bottom_left,
                         const Point2 top_right,
                         const Point2 bottom_right,
                         const Color& color) -> void;

auto draw_filled_rect(Canvas& canvas,
                      const Point2 top_left,
                      const Point2 bottom_left,
                      const Point2 top_right,
                      const Point2 bottom_right,
                      const Color& color) -> void;

auto draw_shaded_rect(Canvas& canvas,
                      const Point2 top_left,
                      const float top_left_intensity,
                      const Point2 bottom_left,
                      const float bottom_left_intensity,
                      const Point2 top_right,
                      const float top_right_intensity,
                      const Point2 bottom_right,
                      const float bottom_right_intensity,
                      const Color& color) -> void;

} // cgfs::rasterizer
