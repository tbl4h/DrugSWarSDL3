//
// Created by mic on 04.06.25.
//

#ifndef SDLERROR_HPP
#define SDLERROR_HPP
#include <cstdint>
#include <format>
#include <string_view>

// C++20 Strong typing z enum class
enum class [[nodiscard]] SDL_Error : std::uint8_t {
    InitFailed = 1,
    WindowCreationFailed,
    RendererCreationFailed,
    RenderingFailed
};

// C++20 constexpr
constexpr std::string_view get_error_prefix(SDL_Error error) noexcept {
    switch (error) {
        case SDL_Error::InitFailed: return "SDL initialization failed";
        case SDL_Error::WindowCreationFailed: return "Window creation failed";
        case SDL_Error::RendererCreationFailed: return "Renderer creation failed";
        case SDL_Error::RenderingFailed: return "Rendering failed";
    }
    return "Unknown error";
}

// POPRAWKA: Bezpieczne formatowanie
[[nodiscard]] std::string error_to_string(SDL_Error error) {
    try {
        auto prefix = get_error_prefix(error);
        auto sdl_error = SDL_GetError();
        return std::format("{}: {}", prefix, sdl_error ? sdl_error : "No error");
    } catch (...) {
        return "Error formatting failed";
    }
}

#endif //SDLERROR_HPP
