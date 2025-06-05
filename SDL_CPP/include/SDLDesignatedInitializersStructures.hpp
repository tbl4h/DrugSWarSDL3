//
// Created by mic on 04.06.25.
//

#ifndef SDLDESIGNATEDINITIALIZERSSTRUCTURES_HPP
#define SDLDESIGNATEDINITIALIZERSSTRUCTURES_HPP
#include <array>
#include <optional>
#include <string>

#include "SDL3/SDL_stdinc.h"

// C++20 Designated initializers structure
struct WindowConfig {
    std::string title{"SDL3 Window"};  // POPRAWKA: std::string zamiast string_view
    int width{800};
    int height{600};
    Uint32 flags{0};
};

struct RenderConfig {
    std::array<Uint8, 4> clear_color{64, 128, 255, 255};
    std::optional<std::string> renderer_name{std::nullopt};  // POPRAWKA: std::string
};


#endif //SDLDESIGNATEDINITIALIZERSSTRUCTURES_HPP
