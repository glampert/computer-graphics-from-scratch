#include "raytracer_demo.hpp"

#include "common/vec3.hpp"
#include "common/mat3.hpp"
#include "common/color.hpp"
#include "common/canvas.hpp"

#include "raytracer/raytrace.hpp"

#include <print>
#include <chrono>

using namespace cgfs;
using namespace cgfs::raytracer;

static auto make_image_save_name(const RaytraceParams& rt_params) -> std::string
{
    std::string name{};
    
    if (rt_params.specular) { name += "specular_"; }
    if (rt_params.shadows) { name += "shadows_"; }
    if (rt_params.reflections) { name += "reflections_"; }
    if (rt_params.refraction) { name += "refraction_"; }

    if (name.empty()) { name += "flat"; }
    if (name.ends_with('_')) { name.pop_back(); }
    
    return "rt_" + name;
}

struct DemoParams final
{
    bool specular{ false };
    bool shadows{ false };
    bool reflections{ false };
    bool refraction{ false };
};

static auto raytracer_demo_internal(const DemoParams& demo_params) -> void
{
    // Scene setup:
    const Sphere spheres[] = {
        {
            .material = {
                .color = Color::kBrightRed,
                .specular = 500.0f, // Shiny
                .reflectiveness = 0.2f, // A bit reflective
                .refractive_index = 0.0f // Opaque / no refraction
            },
            .center = { 0.0f, -1.0f, 3.0f },
            .radius = 1.0f
        },
        {
            .material = {
                .color = Color::kBrightGreen,
                .specular = 500.0f,
                .reflectiveness = 0.4f,
                .refractive_index = 1.1f
            },
            .center = { 0.3f, -0.2f, -1.0f },
            .radius = 0.2f
        },
        {
            .material = {
                .color = Color::kDarkBlue,
                .specular = 700.0f,
                .reflectiveness = 0.4f,
                .refractive_index = 1.1f
            },
            .center = { -0.3f, -0.2f, -1.0f },
            .radius = 0.2f
        },
        {
            .material = {
                .color = Color::kBrightGreen,
                .specular = 10.0f, // Somewhat shiny
                .reflectiveness = 0.3f, // A bit more reflective
                .refractive_index = 1.33f // Refractive index of water
            },
            .center = { -2.0f, 0.0f, 4.0f },
            .radius = 1.0f
        },
        {
            .material = {
                .color = Color::kBrightBlue,
                .specular = 500.0f, // Shiny
                .reflectiveness = 0.4f, // Even more reflective
                .refractive_index = 1.1f // Almost fully transparent (refractive index of air=1)
            },
            .center = { 2.0f, 0.0f, 4.0f },
            .radius = 1.0f
        },
        {
            .material = {
                .color = { 0.0f, 1.0f, 1.0f, 1.0f },
                .specular = 50.0f, // Shiny
                .reflectiveness = 0.2f, // Not reflective
                .refractive_index = 0.0f // Opaque / no refraction
            },
            .center = { 2.0f, 0.0f, 12.0f },
            .radius = 1.2f
        },
        {
            .material = {
                .color = { 1.0f, 0.0f, 1.0f, 1.0f },
                .specular = 50.0f, // Shiny
                .reflectiveness = 0.3f, // Not reflective
                .refractive_index = 0.0f // Opaque / no refraction
            },
            .center = { -2.0f, 0.0f, 12.0f },
            .radius = 1.2f
        },
        {
            .material = {
                .color = Color::kWhite,
                .specular = 50.0f,
                .reflectiveness = 0.3f,
                .refractive_index = 1.33f
            },
            .center = { 0.0f, 1.0f, 14.0f },
            .radius = 1.4f
        },
        {
            .material = {
                .color = Color::kBrightYellow,
                .specular = 1000.0f, // Very shiny
                .reflectiveness = 0.5f, // Half reflective
                .refractive_index = 0.0f // Opaque / no refraction
            },
            .center = { 0.0f, -5001.0f, 0.0f },
            .radius = 5000.0f
        }
    };

    const Light lights[] = {
        {
            .type = Light::Type::kAmbient,
            .intensity = 0.2f
        },
        {
            .type = Light::Type::kPoint,
            .position = { 2.0f, 1.0f, 0.0f },
            .intensity = 0.6f,
        },
        {
            .type = Light::Type::kDirectional,
            .position = { 1.0f, 4.0f, 4.0f },
            .intensity = 0.2f,
        }
    };

    const Scene scene = {
        .spheres = spheres,
        .lights = lights
    };

    const Camera camera = {
        .position = { 0.0f, 0.3f, -3.0f },
        .rotation = Mat3::kIdentity
        // Uncomment these for different camera position and rotation:
        //.position = { 3.0f, 0.0f, 1.0f },
        //.rotation = { 0.7071f, 0.0f, -0.7071f, 0.0f, 1.0f, 0.0f, 0.7071f, 0.0f, 0.7071f }
    };

    const RaytraceParams rt_params = {
        .camera = camera,
        .background_color = Color::kWhite,
        .threading = Threading::k8Threads,
        .specular = demo_params.specular,
        .shadows = demo_params.shadows,
        .reflections = demo_params.reflections,
        .refraction = demo_params.refraction,
        .max_recursion_depth = 4
    };

    Canvas canvas{
        Dims{ 1024, 1024 },
        make_image_save_name(rt_params)
    };

    std::println("Raytracing to image '{}' ...", canvas.name());
    
    const auto start_time = std::chrono::high_resolution_clock::now();
    
    raytrace(canvas, rt_params, scene);
    
    const auto end_time = std::chrono::high_resolution_clock::now();
    const auto time_taken = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    std::println("Raytrace time: {}.", time_taken);

    [[maybe_unused]] const bool result = canvas.present();
    assert(result == true);
}

// Renders 5 different versions of our raytrace test scene with different lighting models.
auto raytracer_demo() -> void
{
    std::println("=== CGFS::raytracer_demo() ===");

    DemoParams demo_params = {};
    raytracer_demo_internal(demo_params);

    demo_params.specular = true;
    raytracer_demo_internal(demo_params);
    
    demo_params.shadows = true;
    raytracer_demo_internal(demo_params);
    
    demo_params.reflections = true;
    raytracer_demo_internal(demo_params);
    
    demo_params.refraction = true;
    raytracer_demo_internal(demo_params);
}
