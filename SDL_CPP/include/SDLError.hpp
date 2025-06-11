//
// Created by mic on 04.06.25.
//

#ifndef SDLERROR_HPP
#define SDLERROR_HPP
#include <cstdint>
#include <format>
#include <string_view>

// C++20 Strong typing z enum class
enum class [[nodiscard]] SDLError : std::uint8_t {
    InitFailed = 1,
    WindowCreationFailed,
    RendererCreationFailed,
    RenderingFailed,
    TextureCreationFailed,
    SDLStateFailed,
};

// C++20 constexpr
constexpr std::string_view get_error_prefix(SDLError error) noexcept {
    switch (error) {
        case SDLError::InitFailed: return "SDL initialization failed";
        case SDLError::WindowCreationFailed: return "Window creation failed";
        case SDLError::RendererCreationFailed: return "Renderer creation failed";
        case SDLError::RenderingFailed: return "Rendering failed";
        case SDLError::TextureCreationFailed: return "Texture creation failed";
        case SDLError::SDLStateFailed: return "SDL state failed";
    }
    return "Unknown error";
}

// POPRAWKA: Bezpieczne formatowanie
[[nodiscard]] std::string error_to_string(SDLError error) {
    try {
        auto prefix = get_error_prefix(error);
        auto sdl_error = SDL_GetError();
        return std::format("{}: {}", prefix, sdl_error ? sdl_error : "No error");
    } catch (...) {
        return "Error formatting failed";
    }
}

#endif //SDLERROR_HPP
