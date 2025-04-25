#pragma once

#include "scene.hpp"
#include "../common/canvas.hpp"

namespace cgfs::raytracer
{

enum class Threading : int
{
    kSingleThread,
    k4Threads,
    k8Threads
};

struct RaytraceParams final
{
    const Camera& camera;
    Color background_color{};
    Threading threading{ Threading::kSingleThread };
    bool specular{ false };
    bool shadows{ false };
    bool reflections{ false };
    bool refraction{ false };
    int max_recursion_depth{ 0 }; // For reflections & refraction; 0=disables reflections/refraction
};

auto raytrace(Canvas& canvas, const RaytraceParams& rt_params, const Scene& scene) -> void;

} // cgfs::raytracer
