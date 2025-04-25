#include "rasterizer_demo.hpp"

#include "rasterizer/draw2d/lines.hpp"
#include "rasterizer/draw2d/rects.hpp"
#include "rasterizer/draw2d/tris.hpp"

#include "rasterizer/texture.hpp"
#include "rasterizer/draw3d.hpp"
#include "rasterizer/mesh.hpp"
#include "rasterizer/scene.hpp"

#include <print>
#include <chrono>

using namespace cgfs;
using namespace cgfs::rasterizer;

static auto shaded_quads_demo() -> void
{
    const auto start_time = std::chrono::high_resolution_clock::now();
    
    std::print("shaded_quads_demo()");
    
    Canvas canvas{
        Dims{ 1024, 1024 },
        "rs_shaded_quads",
        Color::kWhite
    };

    Point2 top_left     = { 101, 301 };
    Point2 bottom_left  = { 101, 101 };
    Point2 top_right    = { 301, 301 };
    Point2 bottom_right = { 301, 101 };

    draw_shaded_rect(canvas,
                     top_left, 1.0f,
                     bottom_left, 0.2f,
                     top_right, 0.2f,
                     bottom_right, 0.2f,
                     Color::kBrightGreen);
    draw_wireframe_rect(canvas, top_left, bottom_left, top_right, bottom_right, Color::kBlack);

    top_left.x     -= 170;
    bottom_left.x  -= 170;
    top_right.x    -= 170;
    bottom_right.x -= 170;

    top_left.y     -= 170;
    bottom_left.y  -= 170;
    top_right.y    -= 170;
    bottom_right.y -= 170;

    draw_shaded_rect(canvas,
                     top_left, 0.2f,
                     bottom_left, 1.0f,
                     top_right, 0.2f,
                     bottom_right, 0.2f,
                     Color::kBrightBlue);
    draw_wireframe_rect(canvas, top_left, bottom_left, top_right, bottom_right, Color::kBlack);

    top_left.x     -= 170;
    bottom_left.x  -= 170;
    top_right.x    -= 170;
    bottom_right.x -= 170;
    
    top_left.y     -= 170;
    bottom_left.y  -= 170;
    top_right.y    -= 170;
    bottom_right.y -= 170;
    
    draw_shaded_rect(canvas,
                     top_left, 0.2f,
                     bottom_left, 0.2f,
                     top_right, 1.0f,
                     bottom_right, 0.2f,
                     Color::kBrightRed);
    draw_wireframe_rect(canvas, top_left, bottom_left, top_right, bottom_right, Color::kBlack);
    
    top_left.x     -= 170;
    bottom_left.x  -= 170;
    top_right.x    -= 170;
    bottom_right.x -= 170;
    
    top_left.y     -= 170;
    bottom_left.y  -= 170;
    top_right.y    -= 170;
    bottom_right.y -= 170;
    
    draw_shaded_rect(canvas,
                     top_left, 0.2f,
                     bottom_left, 0.2f,
                     top_right, 0.2f,
                     bottom_right, 1.0f,
                     Color::kBrightYellow);
    draw_wireframe_rect(canvas, top_left, bottom_left, top_right, bottom_right, Color::kBlack);
    
    [[maybe_unused]] const bool result = canvas.present();
    assert(result == true);
    
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::println(" -> {}.", time_taken);
}

static auto filled_quads_demo() -> void
{
    const auto start_time = std::chrono::high_resolution_clock::now();
    
    std::print("filled_quads_demo()");
    
    Canvas canvas{
        Dims{ 1024, 1024 },
        "rs_filled_quads",
        Color::kWhite
    };

    Point2 top_left     = { -101,  101 };
    Point2 bottom_left  = { -101, -101 };
    Point2 top_right    = {  101,  101 };
    Point2 bottom_right = {  101, -101 };

    draw_filled_rect(canvas, top_left, bottom_left, top_right, bottom_right, Color::kBrightGreen);
    draw_wireframe_rect(canvas, top_left, bottom_left, top_right, bottom_right, Color::kBlack);
    
    top_left.x     -= 100;
    bottom_left.x  -= 100;
    top_right.x    -= 100;
    bottom_right.x -= 100;

    top_left.y     -= 100;
    bottom_left.y  -= 100;
    top_right.y    -= 100;
    bottom_right.y -= 100;

    draw_filled_rect(canvas, top_left, bottom_left, top_right, bottom_right, Color::kBrightBlue);
    draw_wireframe_rect(canvas, top_left, bottom_left, top_right, bottom_right, Color::kBlack);
    
    top_left.x     -= 100;
    bottom_left.x  -= 100;
    top_right.x    -= 100;
    bottom_right.x -= 100;
    
    top_left.y     -= 100;
    bottom_left.y  -= 100;
    top_right.y    -= 100;
    bottom_right.y -= 100;
    
    draw_filled_rect(canvas, top_left, bottom_left, top_right, bottom_right, Color::kBrightRed);
    draw_wireframe_rect(canvas, top_left, bottom_left, top_right, bottom_right, Color::kBlack);
    
    [[maybe_unused]] const bool result = canvas.present();
    assert(result == true);
    
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::println(" -> {}.", time_taken);
}

static auto shaded_triangles_demo() -> void
{
    const auto start_time = std::chrono::high_resolution_clock::now();
    
    std::print("shaded_triangles_demo()");
    
    Canvas canvas{
        Dims{ 1024, 1024 },
        "rs_shaded_triangles",
        Color::kWhite
    };

    Point2 p0 = { -200, -250 };
    Point2 p1 = {  200,  50  };
    Point2 p2 = {  20,   250 };

    draw_shaded_triangle(canvas,
                         p0, 0.4f,
                         p1, 0.2f,
                         p2, 1.0f,
                         Color::kBrightGreen);

    p0.x -= 100;
    p1.x -= 100;
    p2.x -= 100;

    draw_shaded_triangle(canvas,
                         p0, 0.5f,
                         p1, 0.3f,
                         p2, 1.0f,
                         Color::kBrightBlue);

    p0.x -= 100;
    p1.x -= 100;
    p2.x -= 100;

    draw_shaded_triangle(canvas,
                         p0, 0.6f,
                         p1, 0.3f,
                         p2, 1.0f,
                         Color::kBrightRed);
    
    [[maybe_unused]] const bool result = canvas.present();
    assert(result == true);
    
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::println(" -> {}.", time_taken);
}

static auto filled_triangles_demo() -> void
{
    const auto start_time = std::chrono::high_resolution_clock::now();
    
    std::print("filled_triangles_demo()");
    
    Canvas canvas{
        Dims{ 1024, 1024 },
        "rs_filled_triangles",
        Color::kWhite
    };

    Point2 p0 = { -200, -250 };
    Point2 p1 = {  200,  50  };
    Point2 p2 = {  20,   250 };

    draw_filled_triangle(canvas, p0, p1, p2, Color::kBrightGreen);
    draw_wireframe_triangle(canvas, p0, p1, p2, Color::kBlack);

    p0.x -= 100;
    p1.x -= 100;
    p2.x -= 100;
    
    draw_filled_triangle(canvas, p0, p1, p2, Color::kBrightBlue);
    draw_wireframe_triangle(canvas, p0, p1, p2, Color::kBlack);
    
    p0.x -= 100;
    p1.x -= 100;
    p2.x -= 100;
    
    draw_filled_triangle(canvas, p0, p1, p2, Color::kBrightRed);
    draw_wireframe_triangle(canvas, p0, p1, p2, Color::kBlack);
    
    [[maybe_unused]] const bool result = canvas.present();
    assert(result == true);
    
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::println(" -> {}.", time_taken);
}

static auto lines_demo() -> void
{
    const auto start_time = std::chrono::high_resolution_clock::now();
    
    std::print("lines_demo()");
    
    Canvas canvas{
        Dims{ 1024, 1024 },
        "rs_lines",
        Color::kWhite
    };

    // Box with only straight lines:
    draw_line(canvas, { -400, -400 }, {  400, -400 }, Color::kDarkYellow);
    draw_line(canvas, { -400,  400 }, {  400,  400 }, Color::kDarkYellow);
    draw_line(canvas, { -400, -400 }, { -400,  400 }, Color::kDarkYellow);
    draw_line(canvas, {  400, -400 }, {  400,  400 }, Color::kDarkYellow);
    
    // Some diagonal lines inside:
    draw_line(canvas, { -50,  -200 }, { 60,  240 }, Color::kBrightRed);
    draw_line(canvas, { -200, -100 }, { 240, 120 }, Color::kBrightGreen);
    draw_line(canvas, {  0,      0 }, { 50,  50  }, Color::kBlack);

    draw_wireframe_triangle(canvas, { -200, -250 }, { 200, 50 }, { 20, 250 }, Color::kBrightBlue);
    
    [[maybe_unused]] const bool result = canvas.present();
    assert(result == true);
    
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::println(" -> {}.", time_taken);
}

static auto projected_wireframe_cube_demo() -> void
{
    const auto start_time = std::chrono::high_resolution_clock::now();
    
    std::print("projected_wireframe_cube_demo()");
    
    Canvas canvas{
        Dims{ 1024, 1024 },
        "rs_projected_cube",
        Color::kWhite
    };

    const Point3 vA = { -2.0f, -0.5, 5.0f };
    const Point3 vB = { -2.0f,  0.5, 5.0f };
    const Point3 vC = { -1.0f,  0.5, 5.0f };
    const Point3 vD = { -1.0f, -0.5, 5.0f };

    const Point3 vAb = { -2.0f, -0.5, 6.0f };
    const Point3 vBb = { -2.0f,  0.5, 6.0f };
    const Point3 vCb = { -1.0f,  0.5, 6.0f };
    const Point3 vDb = { -1.0f, -0.5, 6.0f };

    draw_line(canvas, canvas.project_vertex(vA), canvas.project_vertex(vB), Color::kBrightBlue);
    draw_line(canvas, canvas.project_vertex(vB), canvas.project_vertex(vC), Color::kBrightBlue);
    draw_line(canvas, canvas.project_vertex(vC), canvas.project_vertex(vD), Color::kBrightBlue);
    draw_line(canvas, canvas.project_vertex(vD), canvas.project_vertex(vA), Color::kBrightBlue);

    draw_line(canvas, canvas.project_vertex(vAb), canvas.project_vertex(vBb), Color::kBrightRed);
    draw_line(canvas, canvas.project_vertex(vBb), canvas.project_vertex(vCb), Color::kBrightRed);
    draw_line(canvas, canvas.project_vertex(vCb), canvas.project_vertex(vDb), Color::kBrightRed);
    draw_line(canvas, canvas.project_vertex(vDb), canvas.project_vertex(vAb), Color::kBrightRed);

    draw_line(canvas, canvas.project_vertex(vA), canvas.project_vertex(vAb), Color::kBrightGreen);
    draw_line(canvas, canvas.project_vertex(vB), canvas.project_vertex(vBb), Color::kBrightGreen);
    draw_line(canvas, canvas.project_vertex(vC), canvas.project_vertex(vCb), Color::kBrightGreen);
    draw_line(canvas, canvas.project_vertex(vD), canvas.project_vertex(vDb), Color::kBrightGreen);
    
    [[maybe_unused]] const bool result = canvas.present();
    assert(result == true);
    
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::println(" -> {}.", time_taken);
}

static auto make_demo_cube_mesh() -> Mesh
{
    static const Texture s_demo_cube_texture{ "assets/crate_texture.png", Texture::Filter::kBilinear };
    const auto* texture = &s_demo_cube_texture;

    return Mesh{
        .vertices = {
            {  1.0f,  1.0f,  1.0f }, // [0]
            { -1.0f,  1.0f,  1.0f }, // [1]
            { -1.0f, -1.0f,  1.0f }, // [2]
            {  1.0f, -1.0f,  1.0f }, // [3]
            {  1.0f,  1.0f, -1.0f }, // [4]
            { -1.0f,  1.0f, -1.0f }, // [5]
            { -1.0f, -1.0f, -1.0f }, // [6]
            {  1.0f, -1.0f, -1.0f }  // [7]
        },
        .normals = {
            {  0.0f,  0.0f,  1.0f }, // [0]
            {  1.0f,  0.0f,  0.0f }, // [1]
            {  0.0f,  0.0f, -1.0f }, // [2]
            { -1.0f,  0.0f,  0.0f }, // [3]
            {  0.0f,  1.0f,  0.0f }, // [4]
            {  0.0f, -1.0f,  0.0f }  // [5]
        },
        .tex_coords = {
            { 0.0f, 0.0f }, // [0]
            { 0.0f, 1.0f }, // [1]
            { 1.0f, 0.0f }, // [2]
            { 1.0f, 1.0f }  // [3]
        },
        .faces = {
            {
                .verts = { 0, 1, 2 },
                .normals = { 0, 0, 0 },
                .tex_coords = { 0, 2, 3 },
                .color = Color::kBrightRed,
                .specular = 50.0f,
                .texture = texture
            },
            {
                .verts = { 0, 2, 3 },
                .normals = { 0, 0, 0 },
                .tex_coords = { 0, 3, 1 },
                .color = Color::kBrightRed,
                .specular = 50.0f,
                .texture = texture
            },
            {
                .verts = { 4, 0, 3 },
                .normals = { 1, 1, 1 },
                .tex_coords = { 0, 2, 3 },
                .color = Color::kBrightGreen,
                .specular = 50.0f,
                .texture = texture
            },
            {
                .verts = { 4, 3, 7 },
                .normals = { 1, 1, 1 },
                .tex_coords = { 0, 3, 1 },
                .color = Color::kBrightGreen,
                .specular = 50.0f,
                .texture = texture
            },
            {
                .verts = { 5, 4, 7 },
                .normals = { 2, 2, 2 },
                .tex_coords = { 0, 2, 3 },
                .color = Color::kBrightBlue,
                .specular = 50.0f,
                .texture = texture
            },
            {
                .verts = { 5, 7, 6 },
                .normals = { 2, 2, 2 },
                .tex_coords = { 0, 3, 1 },
                .color = Color::kBrightBlue,
                .specular = 50.0f,
                .texture = texture
            },
            {
                .verts = { 1, 5, 6 },
                .normals = { 3, 3, 3 },
                .tex_coords = { 0, 2, 3 },
                .color = Color::kBrightYellow,
                .specular = 50.0f,
                .texture = texture
            },
            {
                .verts = { 1, 6, 2 },
                .normals = { 3, 3, 3 },
                .tex_coords = { 0, 3, 1 },
                .color = Color::kBrightYellow,
                .specular = 50.0f,
                .texture = texture
            },
            {
                .verts = { 4, 5, 1 },
                .normals = { 4, 4, 4 },
                .tex_coords = { 0, 2, 3 },
                .color = Color::kPurple,
                .specular = 50.0f,
                .texture = texture
            },
            {
                .verts = { 4, 1, 0 },
                .normals = { 4, 4, 4 },
                .tex_coords = { 1, 3, 0 },
                .color = Color::kPurple,
                .specular = 50.0f,
                .texture = texture
            },
            {
                .verts = { 2, 6, 7 },
                .normals = { 5, 5, 5 },
                .tex_coords = { 0, 2, 3 },
                .color = Color::kCyan,
                .specular = 50.0f,
                .texture = texture
            },
            {
                .verts = { 2, 7, 3 },
                .normals = { 5, 5, 5 },
                .tex_coords = { 0, 3, 1 },
                .color = Color::kCyan,
                .specular = 50.0f,
                .texture = texture
            }
        },
        .bounding_sphere = {
            .center = { 0.0f, 0.0f, 0.0f },
            .radius = std::sqrt(3.0f)
        }
    };
}

static auto make_demo_sphere_mesh(const int divisions = 20,
                                  const Color& color = Color::kBrightGreen) -> Mesh
{
    Mesh sphere{};

    const float f_divisions = static_cast<float>(divisions);
    const float delta_angle = 2.0f * kPI / f_divisions;

    // Generate vertices and normals.
    for (float d = 0.0f; d < (f_divisions + 1.0f); d += 1.0f)
    {
        const float y = (2.0f / f_divisions) * (d - f_divisions / 2.0f);
        const float radius = std::sqrt(1.0f - (y * y));

        for (float i = 0.0f; i < f_divisions; i += 1.0f)
        {
            const Point3 p = {
                radius * std::cos(i * delta_angle),
                y,
                radius * std::sin(i * delta_angle)
            };
            
            sphere.vertices.push_back(p);
            sphere.normals.push_back(p);
        }
    }

    // Generate triangles.
    for (int d = 0; d < divisions; ++d)
    {
        for (int i = 0; i < divisions; ++i)
        {
            const auto i0 = static_cast<std::uint16_t>((d * divisions) + i);
            const auto i1 = static_cast<std::uint16_t>((d + 1) * divisions + (i + 1) % divisions);
            const auto i2 = static_cast<std::uint16_t>(divisions * d + (i + 1) % divisions);
            
            sphere.faces.push_back(Mesh::Face{
                .verts    = { i0, i1, i2 },
                .normals  = { i0, i1, i2 },
                .color    = color,
                .specular = 50.0f
            });

            sphere.faces.push_back(Mesh::Face{
                .verts    = { i0, static_cast<std::uint16_t>(i0 + divisions), i1 },
                .normals  = { i0, static_cast<std::uint16_t>(i0 + divisions), i1 },
                .color    = color,
                .specular = 50.0f
            });
        }
    }
    
    sphere.bounding_sphere.center = { 0.0f, 0.0f, 0.0f };
    sphere.bounding_sphere.radius = 1.0f;

    return sphere;
}

static auto make_clipping_planes_90fov() -> ClippingPlanes
{
    // Clipping planes hardcoded to a 90 degrees field-of-view.
    const float s2 = 1.0f / std::sqrt(2.0f);
    return ClippingPlanes{
        .planes = {{
            { .normal = { 0.0f,  0.0f,  1.0f }, .distance = -1.0f }, // Near
            { .normal = { s2,    0.0f,  s2   }, .distance =  0.0f }, // Left
            { .normal = {-s2,    0.0f,  s2   }, .distance =  0.0f }, // Right
            { .normal = { 0.0f, -s2,    s2   }, .distance =  0.0f }, // Top
            { .normal = { 0.0f,  s2,    s2   }, .distance =  0.0f }  // Bottom
        }}
    };
}

static auto simple_scene_camera_demo() -> void
{
    const auto start_time = std::chrono::high_resolution_clock::now();
    
    std::print("simple_scene_camera_demo()");
    
    Canvas canvas{
        Dims{ 1024, 1024 },
        "rs_simple_scene_camera",
        Color::kWhite
    };

    DepthBuffer depth_buffer{ Dims{ 1024, 1024 } };

    const Mesh cube = make_demo_cube_mesh();

    const Mesh::Instance mesh_instances[] = {
        {
            .mesh = cube,
            .transform = {
                .translation = { -1.5f, 0.0f, 7.0f },
                .rotation = Mat3::kIdentity,
                .scaling = 0.75f
            }
        },
        {
            .mesh = cube,
            .transform = {
                .translation = { 1.25f, 2.5f, 7.5f },
                .rotation = Mat3::rotation_y(175.0f),
                .scaling = 1.0f
            }
        }
    };

    const Camera camera = {
        .position = { -3.0f, 1.0f, 2.0f },
        .rotation = Mat3::rotation_y(-30.0f)
    };

    const Scene scene = {
        .camera = camera,
        .meshes_instances = mesh_instances
    };

    draw_scene(canvas, depth_buffer, scene,
               DrawFlags::kWireframe,
               LightModel::kDisabled,
               ShadeModel::kDisabled);

    [[maybe_unused]] const bool result = canvas.present();
    assert(result == true);
    
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::println(" -> {}.", time_taken);
}

static auto clipping_demo() -> void
{
    const auto start_time = std::chrono::high_resolution_clock::now();
    
    std::print("clipping_demo()");
    
    Canvas canvas{
        Dims{ 1024, 1024 },
        "rs_clipping",
        Color::kWhite
    };

    DepthBuffer depth_buffer{ Dims{ 1024, 1024 } };
    
    const Mesh cube = make_demo_cube_mesh();

    const Mesh::Instance mesh_instances[] = {
        {
            .mesh = cube,
            .transform = {
                .translation = { -1.5f, 0.0f, 7.0f },
                .rotation = Mat3::kIdentity,
                .scaling = 0.75f
            }
        },
        {
            .mesh = cube,
            .transform = {
                .translation = { 1.25f, 2.5f, 7.5f },
                .rotation = Mat3::rotation_y(175.0f),
                .scaling = 1.0f
            }
        },
        {
            // This instance should be totally clipped (behind the camera).
            .mesh = cube,
            .transform = {
                .translation = { 0.0f, 0.0f, -10.0f },
                .rotation = Mat3::rotation_y(175.0f),
                .scaling = 1.0f
            }
        }
    };

    const Camera camera = {
        .position = { -3.0f, 1.0f, 2.0f },
        .rotation = Mat3::rotation_y(-30.0f),
        .clipping_planes = make_clipping_planes_90fov()
    };

    const Scene scene = {
        .camera = camera,
        .meshes_instances = mesh_instances
    };

    draw_scene(canvas, depth_buffer, scene,
               DrawFlags::kWireframe | DrawFlags::kClipping,
               LightModel::kDisabled,
               ShadeModel::kDisabled);

    [[maybe_unused]] const bool result = canvas.present();
    assert(result == true);
    
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::println(" -> {}.", time_taken);
}

static auto depth_buffer_culling_demo() -> void
{
    const auto start_time = std::chrono::high_resolution_clock::now();
    
    std::print("depth_buffer_culling_demo()");
    
    Canvas canvas{
        Dims{ 1024, 1024 },
        "rs_depth_buffer_culling",
        Color::kWhite
    };

    DepthBuffer depth_buffer{ Dims{ 1024, 1024 } };
    
    const Mesh cube = make_demo_cube_mesh();

    const Mesh::Instance mesh_instances[] = {
        {
            .mesh = cube,
            .transform = {
                .translation = { -1.5f, 0.0f, 7.0f },
                .rotation = Mat3::kIdentity,
                .scaling = 0.75f
            }
        },
        {
            .mesh = cube,
            .transform = {
                .translation = { 1.25f, 2.5f, 7.5f },
                .rotation = Mat3::rotation_y(175.0f),
                .scaling = 1.0f
            }
        },
        {
            // This instance should be totally clipped (behind the camera).
            .mesh = cube,
            .transform = {
                .translation = { 0.0f, 0.0f, -10.0f },
                .rotation = Mat3::rotation_y(175.0f),
                .scaling = 1.0f
            }
        }
    };

    const Camera camera = {
        .position = { -3.0f, 1.0f, 2.0f },
        .rotation = Mat3::rotation_y(-30.0f),
        .clipping_planes = make_clipping_planes_90fov()
    };

    const Scene scene = {
        .camera = camera,
        .meshes_instances = mesh_instances
    };

    draw_scene(canvas, depth_buffer, scene,
               DrawFlags::kColorFilled |
               DrawFlags::kOutlines |
               DrawFlags::kDepthTest |
               DrawFlags::kBackFaceCull |
               DrawFlags::kClipping,
               LightModel::kDisabled,
               ShadeModel::kDisabled);
    
    [[maybe_unused]] const bool result = canvas.present();
    assert(result == true);
    
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::println(" -> {}.", time_taken);
}

static const std::string s_shade_model_name[] = {
    "color_fill", // AKA ShadeModel::kDisabled
    "flat",
    "gouraud",
    "phong"
};

static auto lighting_and_shading_demo(const ShadeModel shade_model) -> void
{
    const auto start_time = std::chrono::high_resolution_clock::now();
    
    std::print("lighting_and_shading_demo({})", s_shade_model_name[static_cast<int>(shade_model)]);
    
    Canvas canvas{
        Dims{ 1024, 1024 },
        "rs_lighting_and_shading_" + s_shade_model_name[static_cast<int>(shade_model)],
        Color::kWhite
    };

    DepthBuffer depth_buffer{ Dims{ 1024, 1024 } };
    
    static const Mesh s_cube = make_demo_cube_mesh();
    static const Mesh s_sphere = make_demo_sphere_mesh();

    const Mesh::Instance mesh_instances[] = {
        {
            .mesh = s_cube,
            .transform = {
                .translation = { -1.5f, 0.0f, 7.0f },
                .rotation = Mat3::kIdentity,
                .scaling = 0.75f
            }
        },
        {
            .mesh = s_cube,
            .transform = {
                .translation = { 1.25f, 2.5f, 7.5f },
                .rotation = Mat3::rotation_y(175.0f),
                .scaling = 1.0f
            }
        },
        {
            .mesh = s_sphere,
            .transform = {
                .translation = { 1.75f, -0.5f, 7.0f },
                .rotation = Mat3::kIdentity,
                .scaling = 1.5f
            }
        }
    };

    const Light lights[] = {
        {
            .type = Light::Type::kAmbient,
            .intensity = 0.2f
        },
        {
            .type = Light::Type::kDirectional,
            .position = { -1.0f, 0.0f, 1.0f },
            .intensity = 0.2f
        },
        {
            .type = Light::Type::kPoint,
            .position = { -3.0f, 2.0f, -10.0f },
            .intensity = 0.6f
        }
    };

    const Camera camera = {
        .position = { -3.0f, 1.0f, 2.0f },
        .rotation = Mat3::rotation_y(-30.0f),
        .clipping_planes = make_clipping_planes_90fov()
    };

    const Scene scene = {
        .camera = camera,
        .meshes_instances = mesh_instances,
        .lights = lights
    };

    draw_scene(canvas, depth_buffer, scene,
               ((shade_model == ShadeModel::kFlat) ? DrawFlags::kComputeFaceNormals : 0u) |
               DrawFlags::kColorFilled |
               DrawFlags::kDepthTest |
               DrawFlags::kBackFaceCull |
               DrawFlags::kClipping,
               LightModel::kDiffuse | ((shade_model != ShadeModel::kFlat) ? LightModel::kSpecular : 0u),
               shade_model);

    [[maybe_unused]] const bool result = canvas.present();
    assert(result == true);
    
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::println(" -> {}.", time_taken);
}

static auto texture_mapping_demo(const ShadeModel shade_model) -> void
{
    const auto start_time = std::chrono::high_resolution_clock::now();
    
    std::print("texture_mapping_demo({})", s_shade_model_name[static_cast<int>(shade_model)]);
    
    Canvas canvas{
        Dims{ 1024, 1024 },
        "rs_texture_mapping_" + s_shade_model_name[static_cast<int>(shade_model)],
        Color::kWhite
    };

    DepthBuffer depth_buffer{ Dims{ 1024, 1024 } };
    
    static const Mesh s_cube = make_demo_cube_mesh();

    const Mesh::Instance mesh_instances[] = {
        {
            .mesh = s_cube,
            .transform = {
                .translation = { -1.5f, 0.0f, 7.0f },
                .rotation = Mat3::kIdentity,
                .scaling = 0.75f
            }
        },
        {
            .mesh = s_cube,
            .transform = {
                .translation = { 1.25f, 2.5f, 7.5f },
                .rotation = Mat3::rotation_y(175.0f),
                .scaling = 1.0f
            }
        },
        {
            .mesh = s_cube,
            .transform = {
                .translation = { 1.0f, 0.0f, 5.0f },
                .rotation = Mat3::rotation_y(-30.0f),
                .scaling = 1.0f
            }
        }
    };

    const Light lights[] = {
        {
            .type = Light::Type::kAmbient,
            .intensity = 0.2f
        },
        {
            .type = Light::Type::kDirectional,
            .position = { -1.0f, 0.0f, 1.0f },
            .intensity = 0.2f
        },
        {
            .type = Light::Type::kPoint,
            .position = { -3.0f, 2.0f, -10.0f },
            .intensity = 0.6f
        }
    };

    const Camera camera = {
        .position = { -3.0f, 1.0f, 2.0f },
        .rotation = Mat3::rotation_y(-30.0f),
        .clipping_planes = make_clipping_planes_90fov()
    };

    const Scene scene = {
        .camera = camera,
        .meshes_instances = mesh_instances,
        .lights = lights
    };

    draw_scene(canvas, depth_buffer, scene,
               ((shade_model == ShadeModel::kFlat) ? DrawFlags::kComputeFaceNormals : 0u) |
               DrawFlags::kTextureMapped |
               DrawFlags::kDepthTest |
               DrawFlags::kBackFaceCull |
               DrawFlags::kClipping,
               LightModel::kDiffuse | ((shade_model != ShadeModel::kFlat) ? LightModel::kSpecular : 0u),
               shade_model);

    [[maybe_unused]] const bool result = canvas.present();
    assert(result == true);
    
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::println(" -> {}.", time_taken);
}

enum ObjModelId : int
{
    kObj_Cube,
    kObj_Spot,
    kObj_Cow,
    kObj_Bunny,
    kObj_Teapot,

    kObjCount
};

static const struct ObjModelDesc {
    const std::string filename;
    const float scaling;
    const Color color;
    const float specular;
    const std::string texture;
} s_obj_models[kObjCount] = {
    { .filename = "cube.obj",   .scaling = 1.0f,  .color = Color::kWhite,      .specular = 50.0f, .texture = "crate_texture.png" },
    { .filename = "spot.obj",   .scaling = 1.2f,  .color = Color::kWhite,      .specular = 50.0f, .texture = "spot_texture.png"  },
    { .filename = "cow.obj",    .scaling = 0.2f,  .color = Color::kBrightBlue, .specular = 50.0f, .texture = "" },
    { .filename = "bunny.obj",  .scaling = 15.0f, .color = Color::kPurple,     .specular = 50.0f, .texture = "" },
    { .filename = "teapot.obj", .scaling = 0.5f,  .color = Color::kBrightRed,  .specular = 50.0f, .texture = "" }
};

static auto load_demo_obj_mesh(const ObjModelId obj_id) -> Mesh
{
    static Texture s_textures[kObjCount];

    const bool has_texture = !s_obj_models[obj_id].texture.empty();

    if (has_texture && !s_textures[obj_id].is_valid())
    {
        [[maybe_unused]] const bool is_loaded = s_textures[obj_id].load_from_file(
                        "assets/" + s_obj_models[obj_id].texture,
                        Texture::Filter::kBilinear);
        assert(is_loaded);
    }

    Mesh mesh{};
    
    const std::string filename = "assets/" + s_obj_models[obj_id].filename;
    const float vertex_scaling = s_obj_models[obj_id].scaling;

    [[maybe_unused]] const bool is_loaded = load_obj_mesh_from_file(mesh, filename, vertex_scaling);
    assert(is_loaded);

    mesh.bounding_sphere = compute_bounding_sphere(mesh.vertices);
    
    for (auto& face : mesh.faces)
    {
        face.color    = s_obj_models[obj_id].color;
        face.specular = s_obj_models[obj_id].specular;
        face.texture  = has_texture ? &s_textures[obj_id] : &Texture::kNone;
    }

    return mesh;
}

static auto obj_meshes_demo(const ShadeModel shade_model) -> void
{
    const auto start_time = std::chrono::high_resolution_clock::now();

    std::print("obj_mesh_demo({})", s_shade_model_name[static_cast<int>(shade_model)]);

    Canvas canvas{
        Dims{ 1024, 1024 },
        "rs_obj_mesh_demo_" + s_shade_model_name[static_cast<int>(shade_model)],
        Color::kWhite
    };

    DepthBuffer depth_buffer{ Dims{ 1024, 1024 } };

    // Declare these as statics to avoid reloading the meshes for each ShadeModel call.
    static const Mesh s_cow_mesh    = load_demo_obj_mesh(kObj_Cow);
    static const Mesh s_bunny_mesh  = load_demo_obj_mesh(kObj_Bunny);
    static const Mesh s_spot_mesh   = load_demo_obj_mesh(kObj_Spot);
    static const Mesh s_teapot_mesh = load_demo_obj_mesh(kObj_Teapot);
    static const Mesh s_cube_mesh   = load_demo_obj_mesh(kObj_Cube);

    const Mesh::Instance mesh_instances[] = {
        {
            .mesh = s_cow_mesh,
            .transform = {
                .translation = { -1.8f, 2.5f, 7.0f },
                .rotation = Mat3::rotation_x(15.0f) * Mat3::rotation_y(-60.0f),
                .scaling = 1.0f
            }
        },
        {
            .mesh = s_bunny_mesh,
            .transform = {
                .translation = { 1.5f, 1.0f, 7.5f },
                .rotation = Mat3::rotation_x(15.0f) * Mat3::rotation_y(160.0f),
                .scaling = 1.0f
            }
        },
        {
            .mesh = s_spot_mesh,
            .transform = {
                .translation = { -1.5f, 0.0f, 7.0f },
                .rotation = Mat3::kIdentity,
                .scaling = 1.0f
            }
        },
        {
            .mesh = s_teapot_mesh,
            .transform = {
                .translation = { 1.5f, -1.2f, 7.0f },
                .rotation = Mat3::rotation_y(120.0f),
                .scaling = 1.0f
            }
        },
        {
            .mesh = s_cube_mesh,
            .transform = {
                .translation = { 2.4f, 1.4f, 12.0f },
                .rotation = Mat3::rotation_x(25.0f) * Mat3::rotation_z(-50.0f),
                .scaling = 1.0f
            }
        }
    };

    const Light lights[] = {
        {
            .type = Light::Type::kAmbient,
            .intensity = 0.2f
        },
        {
            .type = Light::Type::kDirectional,
            .position = { -1.0f, 0.0f, 1.0f },
            .intensity = 0.2f
        },
        {
            .type = Light::Type::kPoint,
            .position = { -3.0f, 2.0f, -10.0f },
            .intensity = 0.6f
        }
    };

    const Camera camera = {
        .position = { -3.0f, 1.0f, 2.0f },
        .rotation = Mat3::rotation_y(-30.0f),
        .clipping_planes = make_clipping_planes_90fov()
    };

    const Scene scene = {
        .camera = camera,
        .meshes_instances = mesh_instances,
        .lights = lights
    };

    draw_scene(canvas, depth_buffer, scene,
               ((shade_model == ShadeModel::kFlat) ? DrawFlags::kComputeFaceNormals : 0u) |
               DrawFlags::kTextureMapped |
               DrawFlags::kDepthTest |
               DrawFlags::kBackFaceCull |
               DrawFlags::kClipping,
               LightModel::kDiffuse | ((shade_model != ShadeModel::kFlat) ? LightModel::kSpecular : 0u),
               shade_model);

    [[maybe_unused]] const bool result = canvas.present();
    assert(result == true);
    
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::println(" -> {}.", time_taken);
}

// Runs several tests. Each one saves the result to a different PNG file.
auto rasterizer_demo() -> void
{
    std::println("=== CGFS::rasterizer_demo() ===");

    const auto start_time = std::chrono::high_resolution_clock::now();

    lines_demo();
    filled_triangles_demo();
    shaded_triangles_demo();
    filled_quads_demo();
    shaded_quads_demo();
    projected_wireframe_cube_demo();
    simple_scene_camera_demo();
    clipping_demo();
    depth_buffer_culling_demo();

    lighting_and_shading_demo(ShadeModel::kDisabled);
    lighting_and_shading_demo(ShadeModel::kFlat);
    lighting_and_shading_demo(ShadeModel::kGouraud);
    lighting_and_shading_demo(ShadeModel::kPhong);

    texture_mapping_demo(ShadeModel::kDisabled);
    texture_mapping_demo(ShadeModel::kFlat);
    texture_mapping_demo(ShadeModel::kGouraud);
    texture_mapping_demo(ShadeModel::kPhong);
    
    obj_meshes_demo(ShadeModel::kDisabled);
    obj_meshes_demo(ShadeModel::kFlat);
    obj_meshes_demo(ShadeModel::kGouraud);
    obj_meshes_demo(ShadeModel::kPhong);

    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::println("Rasterizer demo total time: {}.", time_taken);
}
