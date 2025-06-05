//
// Created by mic on 04.06.25.
//

#ifndef SDLDELETERS_HPP
#define SDLDELETERS_HPP
#include <iostream>
#include <SDL3/SDL.h>
#include "SDLResourcesConcepts.hpp"

// C++20 Custom deleters z concepts
template<SDL_Resource T>
struct SDL_CustomDeleter {
    constexpr void operator()(T* resource) const noexcept {
        if constexpr (std::same_as<T, SDL_Window>) {
            if (resource) SDL_DestroyWindow(resource);
        } else if constexpr (std::same_as<T, SDL_Renderer>) {
            if (resource) SDL_DestroyRenderer(resource);
        } else if constexpr (std::same_as<T, SDL_Surface>) {
            if (resource) SDL_DestroySurface(resource);
        } else if constexpr (std::same_as<T, SDL_Texture>) {
            if (resource) SDL_DestroyTexture(resource);
        }
    }
};
#endif //SDLDELETERS_HPP
