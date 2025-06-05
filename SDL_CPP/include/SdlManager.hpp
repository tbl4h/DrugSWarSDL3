//
// Created by mic on 04.06.25.
//

#ifndef SDLMANAGER_HPP
#define SDLMANAGER_HPP
#include <SDL3/SDL.h>
#include <iostream>
#include <utility>


// POPRAWIONY RAII wrapper dla SDL
class SDL_Manager {
private:
    bool m_initialized{false};

public:
    SDL_Manager() noexcept {
        // POPRAWKA: SDL_Init zwraca bool (true = sukces)
        m_initialized = SDL_Init(SDL_INIT_VIDEO);
        if (m_initialized) {
            std::cout << "🔧 SDL zainicjalizowane: " << SDL_GetRevision() << "\n";
        } else {
            std::cerr << "❌ SDL Init failed: " << SDL_GetError() << "\n";
        }
    }

    ~SDL_Manager() noexcept {
        if (m_initialized) [[likely]] {
            std::cout << "🧹 Sprzątanie SDL...\n";
            SDL_Quit();
        }
    }

    // C++20 Three-way comparison - usunięte dla uproszczenia
    SDL_Manager(const SDL_Manager&) = delete;
    SDL_Manager& operator=(const SDL_Manager&) = delete;

    [[nodiscard]] constexpr bool initialized() const noexcept {
        return m_initialized;
    }

    // C++20 Move semantics
    SDL_Manager(SDL_Manager&& other) noexcept
        : m_initialized(std::exchange(other.m_initialized, false)) {}

    SDL_Manager& operator=(SDL_Manager&& other) noexcept {
        if (this != &other) {
            if (m_initialized) {
                SDL_Quit();
            }
            m_initialized = std::exchange(other.m_initialized, false);
        }
        return *this;
    }
};



#endif //SDLMANAGER_HPP
