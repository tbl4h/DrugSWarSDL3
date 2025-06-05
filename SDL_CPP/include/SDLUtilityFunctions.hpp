//
// Created by mic on 04.06.25.
//

#ifndef SDLUTILITYFUNCTIONS_HPP
#define SDLUTILITYFUNCTIONS_HPP
#include <format>
#include <iostream>
#include <utility>

#include "SDLResourcesConcepts.hpp"

// POPRAWKA: Bezpieczne renderowanie
void performRender(SDL_Renderer* renderer, std::span<const Uint8, 4> color) noexcept {
    if (!renderer) return;

    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}

// POPRAWKA: Uproszczone bez ranges (które mogą powodować problemy)
std::vector<std::string> getEventNames() {
    return {
        "SDL_EVENT_QUIT - zamknięcie aplikacji",
        "SDL_EVENT_KEY_DOWN - naciśnięcie klawisza",
        "SDL_EVENT_MOUSE_BUTTON_DOWN - kliknięcie myszy"
    };
}

// C++17 if constexpr
template<typename EventType>
constexpr void handleEvent(const EventType& event) noexcept {
    if constexpr (requires { event.type; event.key; }) {
        if (event.type == SDL_EVENT_KEY_DOWN) {
            std::cout << std::format("⌨️  Klawisz: {}\n", static_cast<int>(event.key.key));
        }
    }
}

#endif //SDLUTILITYFUNCTIONS_HPP
