#include "draw3d.hpp"
#include "draw2d/lines.hpp"
#include "draw2d/tris.hpp"

namespace cgfs::rasterizer
{

// ========================================================
// Lighting & shading:
// ========================================================

// Returns the computed light intensity for the vertex.
static auto compute_lighting(const LightModel::Type light_model,
                             const Point3& vertex,
                             const Vec3& normal,
                             const Camera& camera,
                             const float specular,
                             const std::span<const Light> lights) -> float
{
    if (light_model & LightModel::kDisabled)
    {
        return 1.0f;
    }
    
    float intensity = 0.0f;

    for (const Light& light : lights)
    {
        if (light.type == Light::Type::kAmbient)
        {
            intensity += light.intensity;
            continue;
        }

        Vec3 light_vector{};
        if (light.type == Light::Type::kDirectional)
        {
            const Mat3 camera_matrix = Mat3::transposed(camera.rotation);
            const Vec3 rotated_light = camera_matrix * light.position;
            light_vector = rotated_light; // Position is a vector already for directional lights.
        }
        else if (light.type == Light::Type::kPoint)
        {
            const Mat4 camera_matrix = camera.to_mat4();
            const Vec4 transformed_light = camera_matrix * Vec4{ light.position, 1.0f };
            light_vector = (vertex * -1.0f) + transformed_light.xyz();
        }
        else
        {
            assert(false);
        }

        // Diffuse component.
        if (light_model & LightModel::kDiffuse)
        {
            const float cos_alpha = dot(light_vector, normal) / (length(light_vector) * length(normal));
            if (cos_alpha > 0.0f)
            {
                intensity += cos_alpha * light.intensity;
            }
        }

        // Specular component.
        if (light_model & LightModel::kSpecular)
        {
            const Vec3 reflected = (normal * (2.0f * dot(normal, light_vector))) - light_vector;
            const Vec3 view = camera.position - vertex;

            const float cos_beta = dot(reflected, view) / (length(reflected) * length(view));
            if (cos_beta > 0.0f)
            {
                intensity += std::pow(cos_beta, specular) * light.intensity;
            }
        }
    }

    // NOTE: Must clamp intensity because specular highlights might go above 1.
    return std::clamp(intensity, 0.0f, 1.0f);
}

// ========================================================
// Clipping & culling:
// ========================================================

static auto clip_mesh_bounds(const ClippingPlanes& clipping_planes,
                             const Mesh& mesh, const Mat4& model_view_mtx, const float scale) -> bool
{
    const Vec4 center = model_view_mtx * Vec4{ mesh.bounding_sphere.center, 1.0f };
    const float radius = mesh.bounding_sphere.radius * scale;

    for (auto p = 0; p < ClippingPlanes::kCount; ++p)
    {
        const Plane& plane = clipping_planes.planes[p];
        const float distance = signed_distance(plane, center.xyz());
        if (distance < -radius)
        {
            return true; // Behind plane; clip it.
        }
    }
    
    return false;
}

// Check if any vertex of the triangle is outside the clipping planes.
// Vertices already transformed by the camera and model matrix (model-view).
static auto clip_triangle(const ClippingPlanes& clipping_planes,
                          const Vec4& v0, const Vec4& v1, const Vec4& v2) -> bool
{
    for (auto p = 0; p < ClippingPlanes::kCount; ++p)
    {
        const Plane& plane = clipping_planes.planes[p];

        const int in0 = signed_distance(plane, v0.xyz()) > 0.0f;
        const int in1 = signed_distance(plane, v1.xyz()) > 0.0f;
        const int in2 = signed_distance(plane, v2.xyz()) > 0.0f;

        const int count = in0 + in1 + in2;
        switch (count)
        {
        case 0:
            return true; // Triangle is fully clipped out.
        case 1:
            return true; // The triangle has one vertex in and two out.
        case 2:
            return true; // The triangle has two vertices in and one out.
        default:
            // The triangle is fully in front of the plane.
            break;
        }
    }
    
    return false;
}

static auto compute_triangle_normal(const Vec4& v0, const Vec4& v1, const Vec4& v2) -> Vec3
{
    const Vec4 v0v1 = v1 - v0;
    const Vec4 v0v2 = v2 - v0;
    return normalize(cross(v0v1.xyz(), v0v2.xyz()));
}

static auto is_back_facing_triangle(Vec3* triangle_normal_out, const Vec4& v0, const Vec4& v1, const Vec4& v2) -> bool
{
    *triangle_normal_out = compute_triangle_normal(v0, v1, v2);
    const Vec3 vertex_to_camera = -v0.xyz(); // Why not camera_position - v0.xyz()???
    
    if (dot(vertex_to_camera, *triangle_normal_out) <= 0.0f)
    {
        return true; // Back-facing.
    }
    
    return false; // Front-facing.
}

// ========================================================
// Mesh 3D drawing:
// ========================================================

auto draw_mesh(Canvas& canvas, DepthBuffer& depth_buffer, const DrawMeshParams& params) -> void
{
    const Mesh& mesh = params.mesh;
    const Camera& camera = params.camera;

    const Mat4 model_view_mtx = params.model_view_mtx;
    const Mat3 normal_mtx = Mat3::transposed(camera.rotation) * params.rotation;
    
    const DrawFlags::Type draw_flags = params.draw_flags;
    const LightModel::Type light_model = params.light_model;
    const ShadeModel shade_model = params.shade_model;

    // Transform the bounding sphere and attempt early discard.
    if ((draw_flags & DrawFlags::kClipping) &&
        clip_mesh_bounds(camera.clipping_planes, mesh, model_view_mtx, params.scaling))
    {
        return;
    }

    for (const Mesh::Face& face : mesh.faces)
    {
        const Point3& vert0 = mesh.vertices[face.verts[0]];
        const Point3& vert1 = mesh.vertices[face.verts[1]];
        const Point3& vert2 = mesh.vertices[face.verts[2]];

        const Vec4 transformed_vert0 = model_view_mtx * Vec4{ vert0, 1.0f };
        const Vec4 transformed_vert1 = model_view_mtx * Vec4{ vert1, 1.0f };
        const Vec4 transformed_vert2 = model_view_mtx * Vec4{ vert2, 1.0f };

        if ((draw_flags & DrawFlags::kClipping) &&
            clip_triangle(camera.clipping_planes, transformed_vert0, transformed_vert1, transformed_vert2))
        {
            // NOTE: Instead of discarding the triangle here we should instead
            // check how many vertices are inside the clipping planes and how
            // many are out. If all 3 are out then we discard, otherwise we should
            // split the triangle by the clip plane it intersects. That would result
            // in either one or two new triangles that we would render instead.
            continue;
        }

        Vec3 triangle_normal{};
        if ((draw_flags & DrawFlags::kBackFaceCull) &&
            is_back_facing_triangle(&triangle_normal, transformed_vert0, transformed_vert1, transformed_vert2))
        {
            continue;
        }

        const Point2 projected_vert0 = canvas.project_vertex(transformed_vert0.xyz());
        const Point2 projected_vert1 = canvas.project_vertex(transformed_vert1.xyz());
        const Point2 projected_vert2 = canvas.project_vertex(transformed_vert2.xyz());

        // Light & shading:
        float intensities[3] = {};
        Vec3 normals[3] = {};

        // Flat shading: compute lighting for the entire triangle.
        if (shade_model == ShadeModel::kFlat)
        {
            if (draw_flags & DrawFlags::kComputeFaceNormals)
            {
                normals[0] = triangle_normal;
            }
            else
            {
                normals[0] = normal_mtx * mesh.normals[face.normals[0]];
            }

            const Vec4 center = (transformed_vert0 + transformed_vert1 + transformed_vert2) / 3.0f;
            intensities[0] = compute_lighting(light_model, center.xyz(), normals[0], camera, face.specular, params.lights);
            intensities[1] = intensities[0];
            intensities[2] = intensities[0];
        }
        // Gouraud shading: compute lighting at the vertices.
        else if (shade_model == ShadeModel::kGouraud)
        {
            if (draw_flags & DrawFlags::kComputeFaceNormals)
            {
                normals[0] = triangle_normal;
                normals[1] = triangle_normal;
                normals[2] = triangle_normal;
            }
            else
            {
                normals[0] = normal_mtx * mesh.normals[face.normals[0]];
                normals[1] = normal_mtx * mesh.normals[face.normals[1]];
                normals[2] = normal_mtx * mesh.normals[face.normals[2]];
            }

            intensities[0] = compute_lighting(light_model, transformed_vert0.xyz(), normals[0], camera, face.specular, params.lights);
            intensities[1] = compute_lighting(light_model, transformed_vert1.xyz(), normals[1], camera, face.specular, params.lights);
            intensities[2] = compute_lighting(light_model, transformed_vert2.xyz(), normals[2], camera, face.specular, params.lights);
        }
        // Phong shading: interpolate normal vectors and compute lighting per pixel.
        else if (shade_model == ShadeModel::kPhong)
        {
            if (draw_flags & DrawFlags::kComputeFaceNormals)
            {
                normals[0] = triangle_normal;
                normals[1] = triangle_normal;
                normals[2] = triangle_normal;
            }
            else
            {
                normals[0] = normal_mtx * mesh.normals[face.normals[0]];
                normals[1] = normal_mtx * mesh.normals[face.normals[1]];
                normals[2] = normal_mtx * mesh.normals[face.normals[2]];
            }
        }

        const bool texture_mapped = (draw_flags & DrawFlags::kTextureMapped) && (face.texture != &Texture::kNone);
        const bool color_filled = (draw_flags & DrawFlags::kColorFilled) || (face.texture == &Texture::kNone);
        const bool wireframe = (draw_flags & DrawFlags::kWireframe) && !color_filled && !texture_mapped;

        if (color_filled)
        {
            if (shade_model == ShadeModel::kDisabled)
            {
                if (draw_flags & DrawFlags::kDepthTest)
                {
                    draw_filled_triangle_depth_tested(canvas,
                                                      depth_buffer,
                                                      projected_vert0,
                                                      transformed_vert0.z,
                                                      projected_vert1,
                                                      transformed_vert1.z,
                                                      projected_vert2,
                                                      transformed_vert2.z,
                                                      face.color);
                }
                else
                {
                    draw_filled_triangle(canvas,
                                         projected_vert0,
                                         projected_vert1,
                                         projected_vert2,
                                         face.color);
                }
            }
            else if (shade_model == ShadeModel::kFlat || shade_model == ShadeModel::kGouraud)
            {
                if (draw_flags & DrawFlags::kDepthTest)
                {
                    draw_shaded_triangle_depth_tested(canvas,
                                                      depth_buffer,
                                                      projected_vert0,
                                                      transformed_vert0.z,
                                                      intensities[0],
                                                      projected_vert1,
                                                      transformed_vert1.z,
                                                      intensities[1],
                                                      projected_vert2,
                                                      transformed_vert2.z,
                                                      intensities[2],
                                                      face.color);
                }
                else
                {
                    draw_shaded_triangle(canvas,
                                         projected_vert0,
                                         intensities[0],
                                         projected_vert1,
                                         intensities[1],
                                         projected_vert2,
                                         intensities[2],
                                         face.color);
                }
            }
            else if (shade_model == ShadeModel::kPhong)
            {
                auto compute_lighting_fn = [&](const Point3 point, const Vec3 normal) -> float
                {
                    return compute_lighting(light_model, point, normal, camera, face.specular, params.lights);
                };

                if (draw_flags & DrawFlags::kDepthTest)
                {
                    draw_phong_shaded_triangle_depth_tested(canvas,
                                                            depth_buffer,
                                                            projected_vert0,
                                                            transformed_vert0.z,
                                                            normals[0],
                                                            projected_vert1,
                                                            transformed_vert1.z,
                                                            normals[1],
                                                            projected_vert2,
                                                            transformed_vert2.z,
                                                            normals[2],
                                                            face.color,
                                                            compute_lighting_fn);
                }
                else
                {
                    draw_phong_shaded_triangle(canvas,
                                               projected_vert0,
                                               normals[0],
                                               projected_vert1,
                                               normals[1],
                                               projected_vert2,
                                               normals[2],
                                               face.color,
                                               compute_lighting_fn);
                }
            }
        }
        else if (texture_mapped)
        {
            const TexCoords tex_coords0 = mesh.tex_coords[face.tex_coords[0]];
            const TexCoords tex_coords1 = mesh.tex_coords[face.tex_coords[1]];
            const TexCoords tex_coords2 = mesh.tex_coords[face.tex_coords[2]];

            if (shade_model == ShadeModel::kDisabled)
            {
                if (draw_flags & DrawFlags::kDepthTest)
                {
                    draw_textured_triangle_depth_tested(canvas,
                                                        depth_buffer,
                                                        projected_vert0,
                                                        transformed_vert0.z,
                                                        tex_coords0,
                                                        projected_vert1,
                                                        transformed_vert1.z,
                                                        tex_coords1,
                                                        projected_vert2,
                                                        transformed_vert2.z,
                                                        tex_coords2,
                                                        *face.texture);
                }
                else
                {
                    draw_textured_triangle(canvas,
                                           projected_vert0,
                                           tex_coords0,
                                           projected_vert1,
                                           tex_coords1,
                                           projected_vert2,
                                           tex_coords2,
                                           *face.texture);
                }
            }
            else if (shade_model == ShadeModel::kFlat || shade_model == ShadeModel::kGouraud)
            {
                if (draw_flags & DrawFlags::kDepthTest)
                {
                    draw_shaded_textured_triangle_depth_tested(canvas,
                                                               depth_buffer,
                                                               projected_vert0,
                                                               transformed_vert0.z,
                                                               intensities[0],
                                                               tex_coords0,
                                                               projected_vert1,
                                                               transformed_vert1.z,
                                                               intensities[1],
                                                               tex_coords1,
                                                               projected_vert2,
                                                               transformed_vert2.z,
                                                               intensities[2],
                                                               tex_coords2,
                                                               *face.texture);
                }
                else
                {
                    draw_shaded_textured_triangle(canvas,
                                                  projected_vert0,
                                                  intensities[0],
                                                  tex_coords0,
                                                  projected_vert1,
                                                  intensities[1],
                                                  tex_coords1,
                                                  projected_vert2,
                                                  intensities[2],
                                                  tex_coords2,
                                                  *face.texture);
                }
            }
            else if (shade_model == ShadeModel::kPhong)
            {
                auto compute_lighting_fn = [&](const Point3 point, const Vec3 normal) -> float
                {
                    return compute_lighting(light_model, point, normal, camera, face.specular, params.lights);
                };

                if (draw_flags & DrawFlags::kDepthTest)
                {
                    draw_phong_shaded_textured_triangle_depth_tested(canvas,
                                                                     depth_buffer,
                                                                     projected_vert0,
                                                                     transformed_vert0.z,
                                                                     normals[0],
                                                                     tex_coords0,
                                                                     projected_vert1,
                                                                     transformed_vert1.z,
                                                                     normals[1],
                                                                     tex_coords1,
                                                                     projected_vert2,
                                                                     transformed_vert2.z,
                                                                     normals[2],
                                                                     tex_coords2,
                                                                     *face.texture,
                                                                     compute_lighting_fn);
                }
                else
                {
                    draw_phong_shaded_textured_triangle(canvas,
                                                        projected_vert0,
                                                        normals[0],
                                                        tex_coords0,
                                                        projected_vert1,
                                                        normals[1],
                                                        tex_coords1,
                                                        projected_vert2,
                                                        normals[2],
                                                        tex_coords2,
                                                        *face.texture,
                                                        compute_lighting_fn);
                }
            }
        }
        else if (wireframe)
        {
            draw_wireframe_triangle(canvas,
                                    projected_vert0,
                                    projected_vert1,
                                    projected_vert2,
                                    face.color);
        }

        if (draw_flags & DrawFlags::kOutlines)
        {
            const Color outline_color = face.color * 0.75f;
            draw_line(canvas, projected_vert0, projected_vert1, outline_color);
            draw_line(canvas, projected_vert0, projected_vert2, outline_color);
            draw_line(canvas, projected_vert2, projected_vert1, outline_color);
        }
    }
}

// ========================================================
// Scene 3D drawing:
// ========================================================

auto draw_scene(Canvas& canvas,
                DepthBuffer& depth_buffer,
                const Scene& scene,
                const DrawFlags::Type draw_flags,
                const LightModel::Type light_model,
                const ShadeModel shade_model) -> void
{
    const Mat4 camera_mtx = scene.camera.to_mat4();

    for (const Mesh::Instance& instance : scene.meshes_instances)
    {
        const Mat4 model_to_world_mtx = instance.transform.to_mat4();
        const Mat4 model_view_mtx = camera_mtx * model_to_world_mtx;

        const DrawMeshParams params = {
            .mesh = instance.mesh,
            .camera = scene.camera,
            .lights = scene.lights,

            .draw_flags = draw_flags,
            .light_model = light_model,
            .shade_model = shade_model,

            .model_view_mtx = model_view_mtx,
            .rotation = instance.transform.rotation,
            .scaling = instance.transform.scaling,
        };

        draw_mesh(canvas, depth_buffer, params);
    }
}

} // cgfs::rasterizer
