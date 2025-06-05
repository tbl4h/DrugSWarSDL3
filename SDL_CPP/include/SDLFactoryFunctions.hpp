//
// Created by mic on 04.06.25.
//

#ifndef SDLFACTORYFUNCTIONS_HPP
#define SDLFACTORYFUNCTIONS_HPP
#include <expected>
#include <SDL3/SDL.h>
#include <string>

#include "SDLDesignatedInitializersStructures.hpp"
#include "SDLError.hpp"
#include "SdlManager.hpp"
#include "SDLResourcesAliases.hpp"

// Bezpieczna inicjalizacja SDL
[[nodiscard]] std::expected<std::unique_ptr<SDL_Manager>, SDL_Error>
initializeSDL() noexcept {
    try {
        auto sdl = std::make_unique<SDL_Manager>();
        if (!sdl->initialized()) [[unlikely]] {
            return std::unexpected(SDL_Error::InitFailed);
        }
        return sdl;
    } catch (...) {
        return std::unexpected(SDL_Error::InitFailed);
    }
}

// POPRAWKA: Bezpieczne tworzenie okna
[[nodiscard]] auto createWindow(const WindowConfig& config) noexcept
    -> std::expected<SDL_WindowPtr, SDL_Error> {

    try {
        auto* window = SDL_CreateWindow(
            config.title.c_str(),  // POPRAWKA: c_str() zamiast data()
            config.width,
            config.height,
            config.flags
        );

        if (!window) [[unlikely]] {
            std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
            return std::unexpected(SDL_Error::WindowCreationFailed);
        }

        std::cout << std::format("✅ Okno utworzone: {}x{}\n", config.width, config.height);
        return SDL_WindowPtr{window};
    } catch (...) {
        return std::unexpected(SDL_Error::WindowCreationFailed);
    }
}

[[nodiscard]] auto createRenderer(SDL_Window* window, const RenderConfig& config) noexcept
    -> std::expected<SDL_RendererPtr, SDL_Error> {

    try {
        const char* name = config.renderer_name.has_value()
            ? config.renderer_name->c_str()  // POPRAWKA: c_str()
            : nullptr;

        auto* renderer = SDL_CreateRenderer(window, name);
        if (!renderer) [[unlikely]] {
            std::cerr << "Renderer creation failed: " << SDL_GetError() << "\n";
            return std::unexpected(SDL_Error::RendererCreationFailed);
        }

        std::cout << "✅ Renderer utworzony\n";
        return SDL_RendererPtr{renderer};
    } catch (...) {
        return std::unexpected(SDL_Error::RendererCreationFailed);
    }
}

#endif //SDLFACTORYFUNCTIONS_HPP