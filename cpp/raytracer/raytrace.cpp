#include "raytrace.hpp"

#include <array>
#include <utility>
#include <optional>
#include <future>

namespace cgfs::raytracer
{

// ========================================================
// Helpers:
// ========================================================

struct Ray final
{
    Point3 origin{};
    Vec3 direction{};
    float min_t{};
    float max_t{};
    float refractive_index{};
};

struct ClosestIntersection final
{
    const Material& material;
    Point3 point{};
    Vec3 normal{};
};

static auto reflect(const Vec3 ray_direction, const Vec3 normal) -> Vec3
{
    return normal * (2.0f * dot(normal, ray_direction)) - ray_direction;
}

// Function to compute the refracted ray direction using Snell's Law.
// n1 & n2 are the refraction indices for the ray and surface.
static auto refract(const Vec3 ray_direction, const Vec3 normal, const float n1, const float n2) -> Vec3
{
    const float refractive_ratio = n1 / n2;
    const float cosI = std::fabs(dot(ray_direction, normal));
    const float sinT2 = refractive_ratio * refractive_ratio * (1.0f - cosI * cosI);
    
    if (sinT2 > 1.0f)
    {
        // Total internal reflection, no refraction.
        return { 0.0f, 0.0f, 0.0f };
    }

    const float cosT = std::sqrt(1.0f - sinT2);
    return (ray_direction * refractive_ratio) + (normal * ((refractive_ratio * cosI) - cosT));
};

// Function to compute Reflection Coefficient `r` using Schlick's approximation.
// The value returned by Schlick's approximation ranges from 0 to 1, where:
//   r=0 means full transmission (no reflection) and
//   r=1 means full reflection (no transmission).
static auto fresnel_reflection(const Vec3 ray_direction, const Vec3 normal, const float n1, const float n2) -> float
{
    // Compute the angle between the incident ray and the normal (cosine of the angle).
    const float cosI = std::fabs(dot(ray_direction, normal));

    // Calculate R0 (the reflectance at normal incidence).
    const float r0 = std::pow((n1 - n2) / (n1 + n2), 2.0f);
    
    // Apply Schlick's approximation to compute the reflection coefficient R.
    return r0 + (1.0f - r0) * std::pow(1.0f - cosI, 5.0f);
};

// ========================================================
// Sphere raytracing:
// ========================================================

// Computes the intersection of a ray and a sphere. Returns the values of t for the intersections.
static auto intersect_ray_sphere(const Ray& ray, const Sphere& sphere) -> std::array<float, 2>
{
    const Vec3 oc = ray.origin - sphere.center;

    const float k1 = dot(ray.direction, ray.direction);
    const float k2 = 2.0f * dot(oc, ray.direction);
    const float k3 = dot(oc, oc) - sphere.radius.squared;

    const float discriminant = (k2 * k2) - (4.0f * k1 * k3);
    if (discriminant < 0.0f)
    {
        return { kInfinity, kInfinity };
    }

    const float discriminant_sqrt = std::sqrt(discriminant);
    const float t0 = (-k2 + discriminant_sqrt) / (2.0f * k1);
    const float t1 = (-k2 - discriminant_sqrt) / (2.0f * k1);
    return { t0, t1 };
}

static auto closest_sphere_intersection(const Ray& ray,
                                        std::span<const Sphere> spheres) -> std::optional<ClosestIntersection>
{
    float closest_t = kInfinity;
    const Sphere* closest_sphere = nullptr;

    // We must iterate every sphere in the scene and find the
    // closest one since they are not ordered by distance from the camera.
    for (const Sphere& sphere : spheres)
    {
        const auto ts = intersect_ray_sphere(ray, sphere);

        if (ts[0] < closest_t && ray.min_t < ts[0] && ts[0] < ray.max_t)
        {
            closest_t = ts[0];
            closest_sphere = &sphere;
        }
        
        if (ts[1] < closest_t && ray.min_t < ts[1] && ts[1] < ray.max_t)
        {
            closest_t = ts[1];
            closest_sphere = &sphere;
        }
    }

    if (closest_sphere == nullptr)
    {
        return std::nullopt; // No intersection.
    }

    const Point3 point = ray.origin + (ray.direction * closest_t);
    const Vec3 normal = normalize(point - closest_sphere->center);

    return std::make_optional<ClosestIntersection>({
        .material = closest_sphere->material,
        .point = point,
        .normal = normal
    });
}

static auto is_obstructed_by_sphere(const Ray& ray,
                                    std::span<const Sphere> spheres) -> bool
{
    for (const Sphere& sphere : spheres)
    {
        const auto ts = intersect_ray_sphere(ray, sphere);

        if (ray.min_t < ts[0] && ts[0] < ray.max_t)
        {
            return true;
        }
        
        if (ray.min_t < ts[1] && ts[1] < ray.max_t)
        {
            return true;
        }
    }

    return false;
}

// ========================================================
// Triangle Mesh raytracing:
// ========================================================

struct RayTriangleIntersection final
{
    Vec3 bary_coords{};
    float distance{};
};

// Adapted from the GLM library.
static auto intersect_ray_triangle(const Ray& ray,
                                   const Point3 vert0,
                                   const Point3 vert1,
                                   const Point3 vert2) -> std::optional<RayTriangleIntersection>
{
    // Find vectors for two edges sharing vert0.
    const Vec3 edge1 = vert1 - vert0;
    const Vec3 edge2 = vert2 - vert0;
    
    // Begin calculating determinant - also used to calculate U parameter.
    const Vec3 p = cross(ray.direction, edge2);

    // If determinant is near zero, ray lies in plane of triangle.
    const float det = dot(edge1, p);

    Vec3 perpendicular = {};
    Vec3 bary_coords = {};

    if (det > 0.0f)
    {
        // Calculate distance from vert0 to ray origin.
        const Vec3 dist = ray.origin - vert0;

        // Calculate U parameter and test bounds.
        bary_coords.x = dot(dist, p);
        if (bary_coords.x < 0.0f || bary_coords.x > det)
        {
            return std::nullopt;
        }

        // Prepare to test V parameter.
        perpendicular = cross(dist, edge1);

        // Calculate V parameter and test bounds.
        bary_coords.y = dot(ray.direction, perpendicular);
        if ((bary_coords.y < 0.0f) || ((bary_coords.x + bary_coords.y) > det))
        {
            return std::nullopt;
        }
    }
    else if (det < 0.0f)
    {
        // Calculate distance from vert0 to ray origin.
        const Vec3 dist = ray.origin - vert0;

        // Calculate U parameter and test bounds.
        bary_coords.x = dot(dist, p);
        if ((bary_coords.x > 0.0f) || (bary_coords.x < det))
        {
            return std::nullopt;
        }

        // Prepare to test V parameter.
        perpendicular = cross(dist, edge1);

        // Calculate V parameter and test bounds.
        bary_coords.y = dot(ray.direction, perpendicular);
        if ((bary_coords.y > 0.0f) || (bary_coords.x + bary_coords.y < det))
        {
            return std::nullopt;
        }
    }
    else
    {
        return std::nullopt; // Ray is parallel to the plane of the triangle.
    }

    const float inv_det = 1.0f / det;

    // Calculate distance, ray intersects triangle.
    const float distance = dot(edge2, perpendicular) * inv_det;
    bary_coords *= inv_det;

    return std::make_optional<RayTriangleIntersection>({
        .bary_coords = bary_coords,
        .distance = distance
    });
}

static auto closest_mesh_intersection(const Ray& ray,
                                      std::span<const Mesh> meshes) -> std::optional<ClosestIntersection>
{
    float closest_t = kInfinity;
    Vec3 closest_normal = {};
    const Mesh* closest_mesh = nullptr;

    for (const Mesh& mesh : meshes)
    {
        for (const Mesh::Face& face : mesh.faces)
        {
            const Point3& vert0 = mesh.vertices[face.verts[0]];
            const Point3& vert1 = mesh.vertices[face.verts[1]];
            const Point3& vert2 = mesh.vertices[face.verts[2]];

            if (const auto result = intersect_ray_triangle(ray, vert0, vert1, vert2))
            {
                if (result->distance < closest_t &&
                    ray.min_t < result->distance && result->distance < ray.max_t)
                {
                    closest_t = result->distance;
                    closest_normal = mesh.normals[face.normal];
                    closest_mesh = &mesh;

                    // Prevent null normals.
                    assert(!is_zero(closest_normal));
                    break;
                }
            }
        }
    }

    if (closest_mesh == nullptr)
    {
        return std::nullopt; // No intersection.
    }

    const Point3 point = ray.origin + (ray.direction * closest_t);

    return std::make_optional<ClosestIntersection>({
        .material = closest_mesh->material,
        .point = point,
        .normal = closest_normal
    });
}

static auto is_obstructed_by_mesh(const Ray& ray,
                                  std::span<const Mesh> meshes) -> bool
{
    for (const Mesh& mesh : meshes)
    {
        for (const Mesh::Face& face : mesh.faces)
        {
            const Point3& vert0 = mesh.vertices[face.verts[0]];
            const Point3& vert1 = mesh.vertices[face.verts[1]];
            const Point3& vert2 = mesh.vertices[face.verts[2]];

            if (const auto result = intersect_ray_triangle(ray, vert0, vert1, vert2))
            {
                if (ray.min_t < result->distance && result->distance < ray.max_t)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

// ========================================================
// Generic intersection testing:
// ========================================================

// Find the closest intersection between a ray and the objects in the scene.
static auto closest_intersection(const Ray& ray, const Scene& scene) -> std::optional<ClosestIntersection>
{
    if (const auto result = closest_sphere_intersection(ray, scene.spheres))
    {
        return result;
    }
    
    if (const auto result = closest_mesh_intersection(ray, scene.meshes))
    {
        return result;
    }
    
    return std::nullopt; // No intersection.
}

// Check if ray is obstructed by any of the scene objects, to decide if a point is in shadow.
static auto is_obstructed(const Ray& ray, const Scene& scene) -> bool
{
    if (is_obstructed_by_sphere(ray, scene.spheres))
    {
        return true;
    }
    
    if (is_obstructed_by_mesh(ray, scene.meshes))
    {
        return true;
    }
    
    return false; // Not obstructed.
}

// ========================================================
// Lighting:
// ========================================================

struct ComputedLighting final
{
    float intensity{};
    Color color{};
};

static auto compute_lighting(const RaytraceParams& rt_params, const Scene& scene,
                             const Point3 point, const Vec3 normal, const Vec3 view,
                             const float specular) -> ComputedLighting
{
    int num_lights_computed = 0;
    float sum_intensity = 0.0f;
    Color sum_color = {};

    const float length_n = length(normal);
    const float length_v = length(view);

    for (const Light& light : scene.lights)
    {
        if (light.type == Light::Type::kAmbient)
        {
            sum_intensity += light.intensity;
            sum_color += light.color;
            ++num_lights_computed;
            continue;
        }

        Vec3 l = {};
        float max_t = 0.0f;

        switch (light.type)
        {
        case Light::Type::kDirectional:
            l = light.position;
            max_t = kInfinity;
            break;
                
        case Light::Type::kPoint:
            l = light.position - point;
            max_t = 1.0f;
            break;
                
        default:
            assert(false);
        }

        // Shadow check.
        if (rt_params.shadows)
        {
            const Ray shadow_ray = {
                .origin = point,
                .direction = l,
                .min_t = 0.001f, // Epsilon, conceptually, an "infinitesimaly small" real number.
                .max_t = max_t,
                .refractive_index = 0.0f // Not used for shadows.
            };

            if (is_obstructed(shadow_ray, scene))
            {
                continue;
            }
        }

        // Diffuse light.
        const float n_dot_l = dot(normal, l);
        if (n_dot_l > 0.0f)
        {
            sum_intensity += light.intensity * n_dot_l / (length_n * length(l));
            sum_color += light.color;
            ++num_lights_computed;
            
            // Specular reflection.
            if (rt_params.specular && specular > 0.0f)
            {
                const Vec3 r = reflect(l, normal);
                const float r_dot_v = dot(r, view);
                
                if (r_dot_v > 0.0f)
                {
                    const float spec_contrib = light.intensity * std::pow(r_dot_v / (length(r) * length_v), specular);
                    sum_intensity += spec_contrib;
                }
            }
        }
    }

    // Average of all light colors.
    sum_color /= static_cast<float>(num_lights_computed);

    return {
        // NOTE: Must clamp intensity because specular highlights might go above 1.
        .intensity = std::clamp(sum_intensity, 0.0f, 1.0f),
        .color = sum_color
    };
}

// ========================================================
// Raytracing loop:
// ========================================================

// Traces a ray against the set of objects in the scene and returns a pixel color.
static auto trace_ray(const RaytraceParams& rt_params, const Scene& scene,
                      const Ray& ray, const int max_recursion_depth) -> Color
{
    const auto intersection_result = closest_intersection(ray, scene);
    if (!intersection_result)
    {
        // No hit for this ray; return background color.
        return rt_params.background_color;
    }

    const Material& material = intersection_result->material;
    const Point3 point = intersection_result->point;
    const Vec3 normal = intersection_result->normal;
    const Vec3 view = -ray.direction;

    const auto [light_intensity, light_color] =
        compute_lighting(rt_params, scene, point, normal, view, material.specular);

    assert(is_normalized(light_intensity)); // Light intensity is normalised.
    assert(is_normalized(light_color)); // Light color should also be in the 0-1 range.

    const Color surface_color = material.color * light_color * light_intensity;

    // Debug vertex normals:
    //const Color surface_color = Color::from_vec3(((normal * 0.5f) + Vec3{ 0.5f, 0.5f, 0.5f })) * light_intensity;

    // If we reach the reflection/refraction recursion depth, we'll stop.
    if (max_recursion_depth <= 0)
    {
        return surface_color;
    }
 
    const bool reflection = (rt_params.reflections && material.reflectiveness > 0.0f);
    const bool refraction = (rt_params.refraction && material.refractive_index > 0.0f);
    
    // If the object is not reflective nor refractive (transparent), we are done.
    if (!reflection && !refraction)
    {
        return surface_color;
    }

    const Vec3 refracted_dir = refraction ?
        refract(ray.direction, -normal, ray.refractive_index, material.refractive_index) :
        Vec3{ 0.0f, 0.0f, 0.0f };

    Color final_color = {};

    // If no valid refraction, reflect the ray instead.
    if (is_zero(refracted_dir))
    {
        // Reflection (simplified):
        const Ray reflected_ray = {
            .origin = point,
            .direction = reflect(view, normal),
            .min_t = 0.05f, // Epsilon, tweaked to avoid image artefacts.
            .max_t = kInfinity,
            .refractive_index = material.refractive_index
        };

        const Color reflected_color = trace_ray(rt_params, scene,
                                                reflected_ray, max_recursion_depth - 1);

        final_color = (reflected_color * material.reflectiveness) +
                      (surface_color * (1.0f - material.reflectiveness));
    }
    else // Refraction/transparency:
    {
        // RI must be at least 1 (fully transparent) or higher (opaque).
        assert(material.refractive_index >= 1.0f);

        const Ray refracted_ray = {
            .origin = point,
            .direction = refracted_dir,
            .min_t = 0.05f, // Epsilon, tweaked to avoid image artefacts.
            .max_t = kInfinity,
            .refractive_index = ray.refractive_index
        };

        const Color refracted_color = trace_ray(rt_params, scene,
                                                refracted_ray, max_recursion_depth - 1);

        const Ray reflected_ray = {
            .origin = point,
            .direction = reflect(view, normal),
            .min_t = 0.05f, // Epsilon, tweaked to avoid image artefacts.
            .max_t = kInfinity,
            .refractive_index = ray.refractive_index
        };

        const Color reflected_color = trace_ray(rt_params, scene,
                                                reflected_ray, max_recursion_depth - 1);

        const float r = fresnel_reflection(ray.direction, -normal,
                                           ray.refractive_index, material.refractive_index);
        assert(is_normalized(r));

        // Blend the reflected and refracted colors based on the reflection coefficient 'r'.
        final_color = reflected_color * r + refracted_color * (1.0f - r);
    }

    // Transparency is computed by the raytracer, so alpha can be fixed to 1.
    final_color.a = 1.0f;

    assert(is_normalized(final_color));
    return final_color;
}

static auto trace_ray_at_point(Canvas& canvas, const RaytraceParams& rt_params,
                               const Scene& scene, const Point2 point) -> void
{
    Vec3 camera_direction = canvas.to_viewport(point);
    camera_direction = rt_params.camera.rotation * camera_direction;

    const Ray ray = {
        .origin = rt_params.camera.position,
        .direction = camera_direction,
        .min_t = 1.0f,
        .max_t = kInfinity,
        .refractive_index = 1.0f // Refraction index of air - fully transparent medium.
    };

    const Color color = trace_ray(rt_params, scene, ray,
                                  rt_params.max_recursion_depth);

    canvas.draw_pixel(point, color);
}

// One pass single threaded raytrace.
static auto raytrace_single_thread(Canvas& canvas, const RaytraceParams& rt_params, const Scene& scene) -> void
{
    const auto half_width = canvas.width() / 2;
    const auto half_height = canvas.height() / 2;
    
    for (auto x = -half_width; x < half_width; ++x)
    {
        for (auto y = -half_height; y < half_height; ++y)
        {
            trace_ray_at_point(canvas, rt_params, scene, { x, y });
        }
    }
}

// 4 raytracing threads, canvas is split into 4 sections, each thread handles one section.
static auto raytrace_4_threads(Canvas& canvas, const RaytraceParams& rt_params, const Scene& scene) -> void
{
    const auto half_width = canvas.width() / 2;
    const auto half_height = canvas.height() / 2;
    
    // 3 threads worker + the calling thread.
    std::array<std::future<void>, 3> threads{};

    // Botton left:
    threads[0] = std::async(std::launch::async, [&]() {
        for (auto x = -half_width; x < 0; ++x)
        {
            for (auto y = -half_height; y < 0; ++y)
            {
                trace_ray_at_point(canvas, rt_params, scene, { x, y });
            }
        }
    });

    // Botton right:
    threads[1] = std::async(std::launch::async, [&]() {
        for (auto x = 0; x < half_width; ++x)
        {
            for (auto y = -half_height; y < 0; ++y)
            {
                trace_ray_at_point(canvas, rt_params, scene, { x, y });
            }
        }
    });

    // Top left:
    threads[2] = std::async(std::launch::async, [&]() {
        for (auto x = -half_width; x < 0; ++x)
        {
            for (auto y = 0; y < half_height; ++y)
            {
                trace_ray_at_point(canvas, rt_params, scene, { x, y });
            }
        }
    });

    // Top right:
    // (do it on main thread while waiting for other threads to finish)
    for (auto x = 0; x < half_width; ++x)
    {
        for (auto y = 0; y < half_height; ++y)
        {
            trace_ray_at_point(canvas, rt_params, scene, { x, y });
        }
    }

    // Synchronize threads.
    for (auto& t : threads)
    {
        t.wait();
    }
}

// 8 raytracing threads, canvas is split into 8 sections, each thread handles one section.
static auto raytrace_8_threads(Canvas& canvas, const RaytraceParams& rt_params, const Scene& scene) -> void
{
    const auto half_width = canvas.width() / 2;
    const auto half_height = canvas.height() / 2;
    const auto quarter_width = canvas.width() / 4;

    // 7 threads worker + the calling thread.
    std::array<std::future<void>, 7> threads{};

    // Botton left 1:
    threads[0] = std::async(std::launch::async, [&]() {
        for (auto x = -half_width; x < -quarter_width; ++x)
        {
            for (auto y = -half_height; y < 0; ++y)
            {
                trace_ray_at_point(canvas, rt_params, scene, { x, y });
            }
        }
    });

    // Botton left 2:
    threads[1] = std::async(std::launch::async, [&]() {
        for (auto x = -quarter_width; x < 0; ++x)
        {
            for (auto y = -half_height; y < 0; ++y)
            {
                trace_ray_at_point(canvas, rt_params, scene, { x, y });
            }
        }
    });

    // Botton right 1:
    threads[2] = std::async(std::launch::async, [&]() {
        for (auto x = 0; x < quarter_width; ++x)
        {
            for (auto y = -half_height; y < 0; ++y)
            {
                trace_ray_at_point(canvas, rt_params, scene, { x, y });
            }
        }
    });
    
    // Botton right 2:
    threads[3] = std::async(std::launch::async, [&]() {
        for (auto x = quarter_width; x < half_width; ++x)
        {
            for (auto y = -half_height; y < 0; ++y)
            {
                trace_ray_at_point(canvas, rt_params, scene, { x, y });
            }
        }
    });

    // Top left 1:
    threads[4] = std::async(std::launch::async, [&]() {
        for (auto x = -half_width; x < -quarter_width; ++x)
        {
            for (auto y = 0; y < half_height; ++y)
            {
                trace_ray_at_point(canvas, rt_params, scene, { x, y });
            }
        }
    });

    // Top left 2:
    threads[5] = std::async(std::launch::async, [&]() {
        for (auto x = -quarter_width; x < 0; ++x)
        {
            for (auto y = 0; y < half_height; ++y)
            {
                trace_ray_at_point(canvas, rt_params, scene, { x, y });
            }
        }
    });

    // Top right 1:
    threads[6] = std::async(std::launch::async, [&]() {
        for (auto x = 0; x < quarter_width; ++x)
        {
            for (auto y = 0; y < half_height; ++y)
            {
                trace_ray_at_point(canvas, rt_params, scene, { x, y });
            }
        }
    });

    // Top right 2:
    // (do it on main thread while waiting for other threads to finish)
    for (auto x = quarter_width; x < half_width; ++x)
    {
        for (auto y = 0; y < half_height; ++y)
        {
            trace_ray_at_point(canvas, rt_params, scene, { x, y });
        }
    }

    // Synchronize threads.
    for (auto& t : threads)
    {
        t.wait();
    }
}

// Public API.
auto raytrace(Canvas& canvas, const RaytraceParams& rt_params, const Scene& scene) -> void
{
    switch (rt_params.threading)
    {
    case Threading::kSingleThread:
        raytrace_single_thread(canvas, rt_params, scene);
        break;
        
    case Threading::k4Threads:
        raytrace_4_threads(canvas, rt_params, scene);
        break;
        
    case Threading::k8Threads:
        raytrace_8_threads(canvas, rt_params, scene);
        break;
    }
}

} // cgfs::raytracer
