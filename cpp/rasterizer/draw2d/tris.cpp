#include "tris.hpp"
#include "lines.hpp"

#include "../../common/vec3.hpp"
#include "../../common/vec4.hpp"
#include "../../common/buffer.hpp"

#include <tuple>

namespace cgfs::rasterizer
{

// ========================================================
// Interpolation helpers:
// ========================================================

// Conditional cast:
// - for int type, cast to float.
// - for anything else, do nothing.
template<typename T>
static inline auto to_float(const T& v) -> T
{
    return v;
}

static inline auto to_float(const int i) -> float
{
    return static_cast<float>(i);
}

template<typename T>
static inline auto interpolate(const int i0, const T v0, const int i1, const T v1) -> Buffer<T>
{
    Buffer<T> result{};

    if (i0 == i1)
    {
        result.push_back(v0);
        return result;
    }

    const auto a = to_float(v1 - v0) / to_float(i1 - i0);
    auto d = to_float(v0);

    for (int i = i0; i <= i1; ++i)
    {
        result.push_back(T(d));
        d += a;
    }

    return result;
}

enum class LeftSide : int
{
    k02, k01_12
};

static inline auto interpolate_points(const Point2 p0,
                                      const Point2 p1,
                                      const Point2 p2) -> std::tuple<Buffer<int>, Buffer<int>, LeftSide>
{
    // Interpolate X coordinates over the triangle scanline.
    auto x01 = interpolate(p0.y, p0.x, p1.y, p1.x);
    auto x12 = interpolate(p1.y, p1.x, p2.y, p2.x);
    auto x02 = interpolate(p0.y, p0.x, p2.y, p2.x);

    // Merge the two short sides.
    auto x01_x12 = concatenate(x01.pop_back(), x12);

    // Determine which is left and which is right.
    const auto m = x02.size() / 2;
    if (x02[m] < x01_x12[m])
    {
        return { x02, x01_x12, LeftSide::k02 };
    }
    else
    {
        return { x01_x12, x02, LeftSide::k01_12 };
    }
}

template<typename T>
static inline auto interpolate_attributes(const Point2 p0,
                                          const Point2 p1,
                                          const Point2 p2,
                                          const T a0,
                                          const T a1,
                                          const T a2,
                                          const LeftSide left_side) -> std::pair<Buffer<T>, Buffer<T>>
{
    // Interpolate vertex attributes over the triangle scanline.
    auto a01 = interpolate(p0.y, a0, p1.y, a1);
    auto a12 = interpolate(p1.y, a1, p2.y, a2);
    auto a02 = interpolate(p0.y, a0, p2.y, a2);
    
    // Merge the two short sides.
    auto a01_a12 = concatenate(a01.pop_back(), a12);

    // Determine which is left and which is right.
    switch (left_side)
    {
    case LeftSide::k02:    return { a02, a01_a12 };
    case LeftSide::k01_12: return { a01_a12, a02 };
    }
}

// ========================================================
// Point2 sorting:
// ========================================================

// Type alias for a point bundle: point + any attributes.
template<typename... Attrs>
using PointBundle = std::tuple<Point2&, Attrs&...>;

template<typename... Attrs>
static inline auto bundle(Point2& p, Attrs&... attrs)
{
    return PointBundle<Attrs...>{ p, attrs... };
}

// Generic function to sort 3 point bundles by the lowest Y values of the 3.
template<typename... Attrs>
static inline auto sort_points_by_y(PointBundle<Attrs...>&& p0, PointBundle<Attrs...>&& p1, PointBundle<Attrs...>&& p2)
{
    if (std::get<0>(p1).y < std::get<0>(p0).y) { std::swap(p0, p1); }
    if (std::get<0>(p2).y < std::get<0>(p0).y) { std::swap(p0, p2); }
    if (std::get<0>(p2).y < std::get<0>(p1).y) { std::swap(p1, p2); }
}

static inline auto sort_points_by_y(Point2& p0, Point2& p1, Point2& p2)
{
    if (p1.y < p0.y) { std::swap(p0, p1); }
    if (p2.y < p0.y) { std::swap(p0, p2); }
    if (p2.y < p1.y) { std::swap(p1, p2); }
}

// ========================================================
// Wireframe:
// ========================================================

auto draw_wireframe_triangle(Canvas& canvas,
                             Point2 p0,
                             Point2 p1,
                             Point2 p2,
                             const Color& color) -> void
{
    draw_line(canvas, p0, p1, color);
    draw_line(canvas, p1, p2, color);
    draw_line(canvas, p2, p0, color);
}

// ========================================================
// Filled:
// ========================================================

auto draw_filled_triangle(Canvas& canvas,
                          Point2 p0,
                          Point2 p1,
                          Point2 p2,
                          const Color& color) -> void
{
    // Sort points from bottom to top.
    sort_points_by_y(p0, p1, p2);

    // Compute X coordinates of the edges.
    const auto [x_left, x_right, _] = interpolate_points(p0, p1, p2);

    // Draw horizontal segments.
    for (int y = p0.y; y <= p2.y; ++y)
    {
        const int xl = x_left[y - p0.y];
        const int xr = x_right[y - p0.y];

        for (int x = xl; x <= xr; ++x)
        {
            canvas.draw_pixel({ x, y }, color);
        }
    }
}

auto draw_textured_triangle(Canvas& canvas,
                            Point2 p0,
                            TexCoords t0,
                            Point2 p1,
                            TexCoords t1,
                            Point2 p2,
                            TexCoords t2,
                            const Texture& texture) -> void
{
    // Sort points from bottom to top.
    sort_points_by_y(bundle(p0, t0), bundle(p1, t1), bundle(p2, t2));

    // Compute X coordinates and tex coords of the edges.
    const auto [x_left, x_right, left_side] = interpolate_points(p0, p1, p2);
    const auto [t_left, t_right] = interpolate_attributes(p0, p1, p2, t0, t1, t2, left_side);

    // Draw horizontal segments.
    for (int y = p0.y; y <= p2.y; ++y)
    {
        const int xl = x_left[y - p0.y];
        const int xr = x_right[y - p0.y];
        
        const TexCoords tl = t_left[y - p0.y];
        const TexCoords tr = t_right[y - p0.y];

        // Interpolate attributes for this scanline.
        const auto segment_tex_coords = interpolate(xl, tl, xr, tr);
        
        for (int x = xl; x <= xr; ++x)
        {
            const TexCoords tex_coords = segment_tex_coords[x - xl];
            const Color color = texture.sample_texel(tex_coords);

            canvas.draw_pixel({ x, y }, color);
        }
    }
}

auto draw_filled_triangle_depth_tested(Canvas& canvas,
                                       DepthBuffer& depth_buffer,
                                       Point2 p0,
                                       float z0,
                                       Point2 p1,
                                       float z1,
                                       Point2 p2,
                                       float z2,
                                       const Color& color) -> void
{
    // Sort points from bottom to top.
    sort_points_by_y(bundle(p0, z0), bundle(p1, z1), bundle(p2, z2));

    // Compute attribute values at the edges (note that we use the inverse Z values here).
    const auto [x_left, x_right, left_side] = interpolate_points(p0, p1, p2);
    const auto [z_left, z_right] = interpolate_attributes(p0, p1, p2, 1.0f / z0, 1.0f / z1, 1.0f / z2, left_side);

    // Draw horizontal segments.
    for (int y = p0.y; y <= p2.y; ++y)
    {
        const int xl = x_left[y - p0.y];
        const int xr = x_right[y - p0.y];

        const float zl = z_left[y - p0.y];
        const float zr = z_right[y - p0.y];

        // Interpolate attributes for this scanline.
        const auto segment_zs = interpolate(xl, zl, xr, zr);

        for (int x = xl; x <= xr; ++x)
        {
            const float z_val = segment_zs[x - xl];
            const Point2 pt = { x, y };
            
            if (depth_buffer.test_and_set(pt, z_val))
            {
                canvas.draw_pixel(pt, color);
            }
        }
    }
}

auto draw_textured_triangle_depth_tested(Canvas& canvas,
                                         DepthBuffer& depth_buffer,
                                         Point2 p0,
                                         float z0,
                                         TexCoords t0,
                                         Point2 p1,
                                         float z1,
                                         TexCoords t1,
                                         Point2 p2,
                                         float z2,
                                         TexCoords t2,
                                         const Texture& texture) -> void
{
    // Sort points from bottom to top.
    sort_points_by_y(bundle(p0, z0, t0), bundle(p1, z1, t1), bundle(p2, z2, t2));

    // Compute attribute values at the edges (note that we use the inverse Z values here).
    const auto [x_left, x_right, left_side] = interpolate_points(p0, p1, p2);
    const auto [z_left, z_right] = interpolate_attributes(p0, p1, p2, 1.0f / z0, 1.0f / z1, 1.0f / z2, left_side);

    // Perspective correct texture mapping (divide by Z).
    const auto [t_left, t_right] = interpolate_attributes(p0, p1, p2, t0 / z0, t1 / z1, t2 / z2, left_side);

    // Draw horizontal segments.
    for (int y = p0.y; y <= p2.y; ++y)
    {
        const int xl = x_left[y - p0.y];
        const int xr = x_right[y - p0.y];

        const float zl = z_left[y - p0.y];
        const float zr = z_right[y - p0.y];

        const TexCoords tl = t_left[y - p0.y];
        const TexCoords tr = t_right[y - p0.y];

        // Interpolate attributes for this scanline.
        const auto segment_zs = interpolate(xl, zl, xr, zr);
        const auto segment_tex_coords = interpolate(xl, tl, xr, tr);
        
        for (int x = xl; x <= xr; ++x)
        {
            const float z_val = segment_zs[x - xl];
            const Point2 pt = { x, y };
            
            if (depth_buffer.test_and_set(pt, z_val))
            {
                // Perspective correct: divide by Z.
                const TexCoords tex_coords = segment_tex_coords[x - xl] / z_val;
                const Color color = texture.sample_texel(tex_coords);

                canvas.draw_pixel(pt, color);
            }
        }
    }
}

// ========================================================
// Flat|Gouraud shaded:
// ========================================================

// Implements the equivalent of Gouraud shading (one intensity value per vertex).
auto draw_shaded_triangle(Canvas& canvas,
                          Point2 p0,
                          float i0,
                          Point2 p1,
                          float i1,
                          Point2 p2,
                          float i2,
                          const Color& color) -> void
{
    assert(is_normalized(i0) && is_normalized(i1) && is_normalized(i2));
    
    // Sort points from bottom to top.
    sort_points_by_y(bundle(p0, i0), bundle(p1, i1), bundle(p2, i2));

    // Compute X coordinates and color intensity values of the edges.
    const auto [x_left, x_right, left_side] = interpolate_points(p0, p1, p2);
    const auto [i_left, i_right] = interpolate_attributes(p0, p1, p2, i0, i1, i2, left_side);

    // Draw horizontal segments.
    for (int y = p0.y; y <= p2.y; ++y)
    {
        const int xl = x_left[y - p0.y];
        const int xr = x_right[y - p0.y];
        
        const float il = i_left[y - p0.y];
        const float ir = i_right[y - p0.y];

        const auto segment_intensities = interpolate(xl, il, xr, ir);

        for (int x = xl; x <= xr; ++x)
        {
            const float intensity_val = std::min(segment_intensities[x - xl], 1.0f);
            canvas.draw_pixel({ x, y }, color * intensity_val);
        }
    }
}

auto draw_shaded_textured_triangle(Canvas& canvas,
                                   Point2 p0,
                                   float i0,
                                   TexCoords t0,
                                   Point2 p1,
                                   float i1,
                                   TexCoords t1,
                                   Point2 p2,
                                   float i2,
                                   TexCoords t2,
                                   const Texture& texture) -> void
{
    assert(is_normalized(i0) && is_normalized(i1) && is_normalized(i2));

    // Sort points from bottom to top.
    sort_points_by_y(bundle(p0, i0, t0), bundle(p1, i1, t1), bundle(p2, i2, t2));

    // Compute X coordinates, color intensity and tex coord values of the edges.
    const auto [x_left, x_right, left_side] = interpolate_points(p0, p1, p2);
    const auto [i_left, i_right] = interpolate_attributes(p0, p1, p2, i0, i1, i2, left_side);
    const auto [t_left, t_right] = interpolate_attributes(p0, p1, p2, t0, t1, t2, left_side);

    // Draw horizontal segments.
    for (int y = p0.y; y <= p2.y; ++y)
    {
        const int xl = x_left[y - p0.y];
        const int xr = x_right[y - p0.y];
        
        const float il = i_left[y - p0.y];
        const float ir = i_right[y - p0.y];

        const TexCoords tl = t_left[y - p0.y];
        const TexCoords tr = t_right[y - p0.y];

        // Interpolate attributes for this scanline.
        const auto segment_intensities = interpolate(xl, il, xr, ir);
        const auto segment_tex_coords = interpolate(xl, tl, xr, tr);
        
        for (int x = xl; x <= xr; ++x)
        {
            const float intensity_val = std::min(segment_intensities[x - xl], 1.0f);
            
            const TexCoords tex_coords = segment_tex_coords[x - xl];
            const Color color = texture.sample_texel(tex_coords);

            canvas.draw_pixel({ x, y }, color * intensity_val);
        }
    }
}

// Implements the equivalent of Gouraud shading (one intensity value per vertex).
auto draw_shaded_triangle_depth_tested(Canvas& canvas,
                                       DepthBuffer& depth_buffer,
                                       Point2 p0,
                                       float z0,
                                       float i0,
                                       Point2 p1,
                                       float z1,
                                       float i1,
                                       Point2 p2,
                                       float z2,
                                       float i2,
                                       const Color& color) -> void
{
    assert(is_normalized(i0) && is_normalized(i1) && is_normalized(i2));
    
    // Sort points from bottom to top.
    sort_points_by_y(bundle(p0, z0, i0), bundle(p1, z1, i1), bundle(p2, z2, i2));

    // Compute attribute values at the edges (note that we use the inverse Z values here).
    const auto [x_left, x_right, left_side] = interpolate_points(p0, p1, p2);
    const auto [z_left, z_right] = interpolate_attributes(p0, p1, p2, 1.0f / z0, 1.0f / z1, 1.0f / z2, left_side);
    const auto [i_left, i_right] = interpolate_attributes(p0, p1, p2, i0, i1, i2, left_side);

    // Draw horizontal segments.
    for (int y = p0.y; y <= p2.y; ++y)
    {
        const int xl = x_left[y - p0.y];
        const int xr = x_right[y - p0.y];

        const float zl = z_left[y - p0.y];
        const float zr = z_right[y - p0.y];

        const float il = i_left[y - p0.y];
        const float ir = i_right[y - p0.y];

        // Interpolate attributes for this scanline.
        const auto segment_zs = interpolate(xl, zl, xr, zr);
        const auto segment_intensities = interpolate(xl, il, xr, ir);
        
        for (int x = xl; x <= xr; ++x)
        {
            const float z_val = segment_zs[x - xl];
            const Point2 pt = { x, y };
            
            if (depth_buffer.test_and_set(pt, z_val))
            {
                const float intensity_val = std::min(segment_intensities[x - xl], 1.0f);
                canvas.draw_pixel(pt, color * intensity_val);
            }
        }
    }
}

auto draw_shaded_textured_triangle_depth_tested(Canvas& canvas,
                                                DepthBuffer& depth_buffer,
                                                Point2 p0,
                                                float z0,
                                                float i0,
                                                TexCoords t0,
                                                Point2 p1,
                                                float z1,
                                                float i1,
                                                TexCoords t1,
                                                Point2 p2,
                                                float z2,
                                                float i2,
                                                TexCoords t2,
                                                const Texture& texture) -> void
{
    assert(is_normalized(i0) && is_normalized(i1) && is_normalized(i2));
    
    // Sort points from bottom to top.
    sort_points_by_y(bundle(p0, z0, i0, t0), bundle(p1, z1, i1, t1), bundle(p2, z2, i2, t2));

    // Compute attribute values at the edges (note that we use the inverse Z values here).
    const auto [x_left, x_right, left_side] = interpolate_points(p0, p1, p2);
    const auto [z_left, z_right] = interpolate_attributes(p0, p1, p2, 1.0f / z0, 1.0f / z1, 1.0f / z2, left_side);
    const auto [i_left, i_right] = interpolate_attributes(p0, p1, p2, i0, i1, i2, left_side);
    
    // Perspective correct texture mapping (divide by Z).
    const auto [t_left, t_right] = interpolate_attributes(p0, p1, p2, t0 / z0, t1 / z1, t2 / z2, left_side);

    // Draw horizontal segments.
    for (int y = p0.y; y <= p2.y; ++y)
    {
        const int xl = x_left[y - p0.y];
        const int xr = x_right[y - p0.y];

        const float zl = z_left[y - p0.y];
        const float zr = z_right[y - p0.y];

        const float il = i_left[y - p0.y];
        const float ir = i_right[y - p0.y];

        const TexCoords tl = t_left[y - p0.y];
        const TexCoords tr = t_right[y - p0.y];
        
        // Interpolate attributes for this scanline.
        const auto segment_zs = interpolate(xl, zl, xr, zr);
        const auto segment_intensities = interpolate(xl, il, xr, ir);
        const auto segment_tex_coords = interpolate(xl, tl, xr, tr);

        for (int x = xl; x <= xr; ++x)
        {
            const float z_val = segment_zs[x - xl];
            const Point2 pt = { x, y };
            
            if (depth_buffer.test_and_set(pt, z_val))
            {
                const float intensity_val = std::min(segment_intensities[x - xl], 1.0f);
                
                // Perspective correct: divide by Z.
                const TexCoords tex_coords = segment_tex_coords[x - xl] / z_val;
                const Color color = texture.sample_texel(tex_coords);
                
                canvas.draw_pixel(pt, color * intensity_val);
            }
        }
    }
}

// ========================================================
// Phong shaded:
// ========================================================

auto draw_phong_shaded_triangle(Canvas& canvas,
                                Point2 p0,
                                Vec3 n0,
                                Point2 p1,
                                Vec3 n1,
                                Point2 p2,
                                Vec3 n2,
                                const Color& color,
                                PhongLightingFunc&& compute_lighting_fn) -> void
{
    // Sort points from bottom to top.
    sort_points_by_y(bundle(p0, n0), bundle(p1, n1), bundle(p2, n2));

    // Compute attribute values at the edges.
    const auto [x_left, x_right, left_side] = interpolate_points(p0, p1, p2);
    const auto [n_left, n_right] = interpolate_attributes(p0, p1, p2, n0, n1, n2, left_side);

    // Draw horizontal segments.
    for (int y = p0.y; y <= p2.y; ++y)
    {
        const int xl = x_left[y - p0.y];
        const int xr = x_right[y - p0.y];

        const Vec3 nl = n_left[y - p0.y];
        const Vec3 nr = n_right[y - p0.y];

        // Interpolate attributes for this scanline.
        const auto segment_normals = interpolate(xl, nl, xr, nr);

        for (int x = xl; x <= xr; ++x)
        {
            const Point2 pt = { x, y };
            
            const float z_val = 1.0f;
            const Point3 vertex = canvas.unproject_vertex(pt, z_val);
            const Vec3 normal = segment_normals[x - xl];
            const float intensity_val = std::min(compute_lighting_fn(vertex, normal), 1.0f);
            
            canvas.draw_pixel(pt, color * intensity_val);
        }
    }
}

auto draw_phong_shaded_textured_triangle(Canvas& canvas,
                                         Point2 p0,
                                         Vec3 n0,
                                         TexCoords t0,
                                         Point2 p1,
                                         Vec3 n1,
                                         TexCoords t1,
                                         Point2 p2,
                                         Vec3 n2,
                                         TexCoords t2,
                                         const Texture& texture,
                                         PhongLightingFunc&& compute_lighting_fn) -> void
{
    // Sort points from bottom to top.
    sort_points_by_y(bundle(p0, n0, t0), bundle(p1, n1, t1), bundle(p2, n2, t2));

    // Compute attribute values at the edges.
    const auto [x_left, x_right, left_side] = interpolate_points(p0, p1, p2);
    const auto [n_left, n_right] = interpolate_attributes(p0, p1, p2, n0, n1, n2, left_side);
    const auto [t_left, t_right] = interpolate_attributes(p0, p1, p2, t0, t1, t2, left_side);

    // Draw horizontal segments.
    for (int y = p0.y; y <= p2.y; ++y)
    {
        const int xl = x_left[y - p0.y];
        const int xr = x_right[y - p0.y];

        const Vec3 nl = n_left[y - p0.y];
        const Vec3 nr = n_right[y - p0.y];

        const TexCoords tl = t_left[y - p0.y];
        const TexCoords tr = t_right[y - p0.y];
        
        // Interpolate attributes for this scanline.
        const auto segment_normals = interpolate(xl, nl, xr, nr);
        const auto segment_tex_coords = interpolate(xl, tl, xr, tr);
        
        for (int x = xl; x <= xr; ++x)
        {
            const Point2 pt = { x, y };
            
            const float z_val = 1.0f;
            const Point3 vertex = canvas.unproject_vertex(pt, z_val);
            const Vec3 normal = segment_normals[x - xl];
            const float intensity_val = std::min(compute_lighting_fn(vertex, normal), 1.0f);
            
            const TexCoords tex_coords = segment_tex_coords[x - xl];
            const Color color = texture.sample_texel(tex_coords);
            
            canvas.draw_pixel(pt, color * intensity_val);
        }
    }
}

auto draw_phong_shaded_triangle_depth_tested(Canvas& canvas,
                                             DepthBuffer& depth_buffer,
                                             Point2 p0,
                                             float z0,
                                             Vec3 n0,
                                             Point2 p1,
                                             float z1,
                                             Vec3 n1,
                                             Point2 p2,
                                             float z2,
                                             Vec3 n2,
                                             const Color& color,
                                             PhongLightingFunc&& compute_lighting_fn) -> void
{
    // Sort points from bottom to top.
    sort_points_by_y(bundle(p0, z0, n0), bundle(p1, z1, n1), bundle(p2, z2, n2));

    // Compute attribute values at the edges (note that we use the inverse Z values here).
    const auto [x_left, x_right, left_side] = interpolate_points(p0, p1, p2);
    const auto [n_left, n_right] = interpolate_attributes(p0, p1, p2, n0, n1, n2, left_side);
    const auto [z_left, z_right] = interpolate_attributes(p0, p1, p2, 1.0f / z0, 1.0f / z1, 1.0f / z2, left_side);

    // Draw horizontal segments.
    for (int y = p0.y; y <= p2.y; ++y)
    {
        const int xl = x_left[y - p0.y];
        const int xr = x_right[y - p0.y];

        const float zl = z_left[y - p0.y];
        const float zr = z_right[y - p0.y];

        const Vec3 nl = n_left[y - p0.y];
        const Vec3 nr = n_right[y - p0.y];

        // Interpolate attributes for this scanline.
        const auto segment_zs = interpolate(xl, zl, xr, zr);
        const auto segment_normals = interpolate(xl, nl, xr, nr);

        for (int x = xl; x <= xr; ++x)
        {
            const float z_val = segment_zs[x - xl];
            const Point2 pt = { x, y };
            
            if (depth_buffer.test_and_set(pt, z_val))
            {
                const Point3 vertex = canvas.unproject_vertex(pt, z_val);
                const Vec3 normal = segment_normals[x - xl];
                const float intensity_val = std::min(compute_lighting_fn(vertex, normal), 1.0f);

                canvas.draw_pixel(pt, color * intensity_val);
            }
        }
    }
}

auto draw_phong_shaded_textured_triangle_depth_tested(Canvas& canvas,
                                                      DepthBuffer& depth_buffer,
                                                      Point2 p0,
                                                      float z0,
                                                      Vec3 n0,
                                                      TexCoords t0,
                                                      Point2 p1,
                                                      float z1,
                                                      Vec3 n1,
                                                      TexCoords t1,
                                                      Point2 p2,
                                                      float z2,
                                                      Vec3 n2,
                                                      TexCoords t2,
                                                      const Texture& texture,
                                                      PhongLightingFunc&& compute_lighting_fn) -> void
{
    // Sort points from bottom to top.
    sort_points_by_y(bundle(p0, z0, n0, t0), bundle(p1, z1, n1, t1), bundle(p2, z2, n2, t2));

    // Compute attribute values at the edges (note that we use the inverse Z values here).
    const auto [x_left, x_right, left_side] = interpolate_points(p0, p1, p2);
    const auto [n_left, n_right] = interpolate_attributes(p0, p1, p2, n0, n1, n2, left_side);
    const auto [z_left, z_right] = interpolate_attributes(p0, p1, p2, 1.0f / z0, 1.0f / z1, 1.0f / z2, left_side);

    // Perspective correct texture mapping (divide by Z).
    const auto [t_left, t_right] = interpolate_attributes(p0, p1, p2, t0 / z0, t1 / z1, t2 / z2, left_side);

    // Draw horizontal segments.
    for (int y = p0.y; y <= p2.y; ++y)
    {
        const int xl = x_left[y - p0.y];
        const int xr = x_right[y - p0.y];

        const float zl = z_left[y - p0.y];
        const float zr = z_right[y - p0.y];

        const Vec3 nl = n_left[y - p0.y];
        const Vec3 nr = n_right[y - p0.y];

        const TexCoords tl = t_left[y - p0.y];
        const TexCoords tr = t_right[y - p0.y];
        
        // Interpolate attributes for this scanline.
        const auto segment_zs = interpolate(xl, zl, xr, zr);
        const auto segment_normals = interpolate(xl, nl, xr, nr);
        const auto segment_tex_coords = interpolate(xl, tl, xr, tr);

        for (int x = xl; x <= xr; ++x)
        {
            const float z_val = segment_zs[x - xl];
            const Point2 pt = { x, y };
            
            if (depth_buffer.test_and_set(pt, z_val))
            {
                const Point3 vertex = canvas.unproject_vertex(pt, z_val);
                const Vec3 normal = segment_normals[x - xl];
                const float intensity_val = std::min(compute_lighting_fn(vertex, normal), 1.0f);

                // Perspective correct: divide by Z.
                const TexCoords tex_coords = segment_tex_coords[x - xl] / z_val;
                const Color color = texture.sample_texel(tex_coords);

                canvas.draw_pixel(pt, color * intensity_val);
            }
        }
    }
}

} // cgfs::rasterizer
