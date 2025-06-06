//
// Created by mic on 04.06.25.
//

#ifndef SDLUTILITYFUNCTIONS_HPP
#define SDLUTILITYFUNCTIONS_HPP
#include <format>
#include <iostream>
#include <utility>
#include <xmmintrin.h>

#include "SDLResourcesConcepts.hpp"

// POPRAWKA: Bezpieczne renderowanie
void performRender(SDL_Renderer *renderer, std::span<const Uint8, 4> color) noexcept {
    if (!renderer) return;

    SDL_SetRenderDrawColor(renderer, color[0], color[1], color[2], color[3]);
    SDL_RenderClear(renderer);
    //SDL_RenderPresent(renderer);
}

void renderTexture(SDL_Renderer *renderer, SDL_Texture *texture, float x, float y) noexcept {
    if (!renderer || !texture) return;

    // Prefetch renderer do cache L1
    _mm_prefetch(reinterpret_cast<const char*>(renderer), _MM_HINT_T0);


    SDL_FRect dst_rect{x, y, 32, 32};
    //SDL_GetTextureSize(texture, &dst_rect.w, &dst_rect.h);

    SDL_RenderTexture(renderer, texture, &dst_rect, &dst_rect);
}

inline void render_frame(SDL_Renderer *renderer, const std::vector<SDL_Texture *> &textures) {
    // Wszystkie operacje SDL w jednym miejscu
    SDL_SetRenderDrawColor(renderer, 0, 0, 100, 0);
    SDL_RenderClear(renderer);
    for (auto *texture: textures) {
        SDL_RenderTexture(renderer, texture, nullptr, nullptr);
    }

    SDL_RenderPresent(renderer);
}


void warm_up_cache(SDL_Renderer *renderer) {
    // Dummy operacje - załadują renderer do cache
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    // Nie wywołujemy SDL_RenderPresent() - to tylko rozgrzewka
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
constexpr void handleEvent(const EventType &event) noexcept {
    if constexpr (requires { event.type; event.key; }) {
        if (event.type == SDL_EVENT_KEY_DOWN) {
            std::cout << std::format("⌨️  Klawisz: {}\n", static_cast<int>(event.key.key));
        }
    }
}

#endif //SDLUTILITYFUNCTIONS_HPP
