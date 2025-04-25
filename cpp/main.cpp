// ===============================================================
// Simple raytracer and software rasterizer as described in
//        "Computer Graphics from Scratch" - CGFS
// https://www.gabrielgambetta.com/computer-graphics-from-scratch
// ===============================================================

#include "raytracer_demo.hpp"
#include "rasterizer_demo.hpp"

#include <print>
#include <unistd.h>

static auto print_working_dir() -> void
{
    // Print current working directory:
    char cwd_buffer[1024] = {};
    getcwd(cwd_buffer, std::size(cwd_buffer));
    std::println("CWD:\n{}\n", cwd_buffer);
}

auto main([[maybe_unused]] const int argc,
          [[maybe_unused]] const char* argv[]) -> int
{
    print_working_dir();
    raytracer_demo();
    rasterizer_demo();
}
