//
// Created by mic on 04.06.25.
//

#ifndef SDLFACTORYFUNCTIONS_HPP
#define SDLFACTORYFUNCTIONS_HPP
#include <expected>
#include <filesystem>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>

#include "SDLArgumentsStructure.hpp"
#include "SDLError.hpp"
#include "SdlManager.hpp"
#include "SDLResourcesAliases.hpp"

// Bezpieczna inicjalizacja SDL
[[nodiscard]] std::expected<std::unique_ptr<SDL_Manager>, SDLError>
initializeSDL() noexcept {
    try {
        auto sdl = std::make_unique<SDL_Manager>();
        if (!sdl->initialized()) [[unlikely]] {
            return std::unexpected(SDLError::InitFailed);
        }
        return sdl;
    } catch (...) {
        return std::unexpected(SDLError::InitFailed);
    }
}

// POPRAWKA: Bezpieczne tworzenie okna
[[nodiscard]] auto createWindow(const WindowConfig &config) noexcept
    -> std::expected<SDL_WindowPtr, SDLError> {
    try {
        auto *window = SDL_CreateWindow(
            config.title.c_str(), // POPRAWKA: c_str() zamiast data()
            config.width,
            config.height,
            config.flags
        );

        if (!window) [[unlikely]] {
            std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
            return std::unexpected(SDLError::WindowCreationFailed);
        }

        std::cout << std::format("✅ Okno utworzone: {}x{}\n", config.width, config.height);
        return SDL_WindowPtr{window};
    } catch (...) {
        return std::unexpected(SDLError::WindowCreationFailed);
    }
}

[[nodiscard]] auto createRenderer(SDL_Window *window, const RenderConfig &config) noexcept
    -> std::expected<SDL_RendererPtr, SDLError> {
    try {
        const char *name = config.renderer_name.has_value()
                               ? config.renderer_name->c_str() // POPRAWKA: c_str()
                               : nullptr;

        auto *renderer = SDL_CreateRenderer(window, name);
        if (!renderer) [[unlikely]] {
            std::cerr << "Renderer creation failed: " << SDL_GetError() << "\n";
            return std::unexpected(SDLError::RendererCreationFailed);
        }

        std::cout << "✅ Renderer utworzony\n";
        return SDL_RendererPtr{renderer};
    } catch (...) {
        return std::unexpected(SDLError::RendererCreationFailed);
    }
}

[[nodiscard]] auto createTexture(SDL_Renderer *renderer,
                                 const std::string &file_path) noexcept -> std::expected<SDL_TexturePtr, SDLError> {
    try {
        if (!std::filesystem::exists(file_path)) {
            std::cerr << "File does not exist: " << file_path << "\n";
            return std::unexpected(SDLError::TextureCreationFailed);
        }

        auto *texture = IMG_LoadTexture(renderer, file_path.c_str());
        if (!texture) [[unlikely]] {
            std::cerr << "Texture creation failed: " << SDL_GetError() << "\n";
            return std::unexpected(SDLError::TextureCreationFailed);
        }

        std::cout << "✅ Tekstura utworzona\n";
        return SDL_TexturePtr{texture};
    } catch (...) {
        return std::unexpected(SDLError::TextureCreationFailed);
    }
}

#endif //SDLFACTORYFUNCTIONS_HPP
