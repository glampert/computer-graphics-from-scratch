#pragma once

#include "../../common/canvas.hpp"
#include "../depth_buffer.hpp"
#include "../texture.hpp"
#include <functional>

namespace cgfs::rasterizer
{

// ==========
// WIREFRAME:
// ==========

auto draw_wireframe_triangle(Canvas& canvas,
                             const Point2 p0,
                             const Point2 p1,
                             const Point2 p2,
                             const Color& color) -> void;

// ========================
// COLOR FILLED | TEXTURED:
// ========================

auto draw_filled_triangle(Canvas& canvas,
                          const Point2 p0,
                          const Point2 p1,
                          const Point2 p2,
                          const Color& color) -> void;

auto draw_textured_triangle(Canvas& canvas,
                            const Point2 p0,
                            const TexCoords t0,
                            const Point2 p1,
                            const TexCoords t1,
                            const Point2 p2,
                            const TexCoords t2,
                            const Texture& texture) -> void;

auto draw_filled_triangle_depth_tested(Canvas& canvas,
                                       DepthBuffer& depth_buffer,
                                       const Point2 p0,
                                       const float z0,
                                       const Point2 p1,
                                       const float z1,
                                       const Point2 p2,
                                       const float z2,
                                       const Color& color) -> void;

auto draw_textured_triangle_depth_tested(Canvas& canvas,
                                         DepthBuffer& depth_buffer,
                                         const Point2 p0,
                                         const float z0,
                                         const TexCoords t0,
                                         const Point2 p1,
                                         const float z1,
                                         const TexCoords t1,
                                         const Point2 p2,
                                         const float z2,
                                         const TexCoords t2,
                                         const Texture& texture) -> void;

// =======================
// FLAT | GOURAUD SHADING:
// =======================

auto draw_shaded_triangle(Canvas& canvas,
                          const Point2 p0,
                          const float i0,
                          const Point2 p1,
                          const float i1,
                          const Point2 p2,
                          const float i2,
                          const Color& color) -> void;

auto draw_shaded_textured_triangle(Canvas& canvas,
                                   const Point2 p0,
                                   const float i0,
                                   const TexCoords t0,
                                   const Point2 p1,
                                   const float i1,
                                   const TexCoords t1,
                                   const Point2 p2,
                                   const float i2,
                                   const TexCoords t2,
                                   const Texture& texture) -> void;

auto draw_shaded_triangle_depth_tested(Canvas& canvas,
                                       DepthBuffer& depth_buffer,
                                       const Point2 p0,
                                       const float z0,
                                       const float i0,
                                       const Point2 p1,
                                       const float z1,
                                       const float i1,
                                       const Point2 p2,
                                       const float z2,
                                       const float i2,
                                       const Color& color) -> void;

auto draw_shaded_textured_triangle_depth_tested(Canvas& canvas,
                                                DepthBuffer& depth_buffer,
                                                const Point2 p0,
                                                const float z0,
                                                const float i0,
                                                const TexCoords t0,
                                                const Point2 p1,
                                                const float z1,
                                                const float i1,
                                                const TexCoords t1,
                                                const Point2 p2,
                                                const float z2,
                                                const float i2,
                                                const TexCoords t2,
                                                const Texture& texture) -> void;

// ==============
// PHONG SHADING:
// ==============

// Given a point and a normal, compute and return the light intensity for it.
using PhongLightingFunc = std::function<float(Point3, Vec3)>;

auto draw_phong_shaded_triangle(Canvas& canvas,
                                const Point2 p0,
                                const Vec3 n0,
                                const Point2 p1,
                                const Vec3 n1,
                                const Point2 p2,
                                const Vec3 n2,
                                const Color& color,
                                PhongLightingFunc&& compute_lighting_fn) -> void;

auto draw_phong_shaded_textured_triangle(Canvas& canvas,
                                         const Point2 p0,
                                         const Vec3 n0,
                                         const TexCoords t0,
                                         const Point2 p1,
                                         const Vec3 n1,
                                         const TexCoords t1,
                                         const Point2 p2,
                                         const Vec3 n2,
                                         const TexCoords t2,
                                         const Texture& texture,
                                         PhongLightingFunc&& compute_lighting_fn) -> void;

auto draw_phong_shaded_triangle_depth_tested(Canvas& canvas,
                                             DepthBuffer& depth_buffer,
                                             const Point2 p0,
                                             const float z0,
                                             const Vec3 n0,
                                             const Point2 p1,
                                             const float z1,
                                             const Vec3 n1,
                                             const Point2 p2,
                                             const float z2,
                                             const Vec3 n2,
                                             const Color& color,
                                             PhongLightingFunc&& compute_lighting_fn) -> void;

auto draw_phong_shaded_textured_triangle_depth_tested(Canvas& canvas,
                                                      DepthBuffer& depth_buffer,
                                                      const Point2 p0,
                                                      const float z0,
                                                      const Vec3 n0,
                                                      const TexCoords t0,
                                                      const Point2 p1,
                                                      const float z1,
                                                      const Vec3 n1,
                                                      const TexCoords t1,
                                                      const Point2 p2,
                                                      const float z2,
                                                      const Vec3 n2,
                                                      const TexCoords t2,
                                                      const Texture& texture,
                                                      PhongLightingFunc&& compute_lighting_fn) -> void;

} // cgfs::rasterizer
