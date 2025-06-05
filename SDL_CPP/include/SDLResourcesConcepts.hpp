//
// Created by mic on 04.06.25.
//

#ifndef SDLCONCEPTS_HPP
#define SDLCONCEPTS_HPP

#include <SDL3/SDL.h>
#include <concepts>
#include <type_traits>
// C++20 Concepts
template<typename T>
concept SDL_Resource = requires(T* ptr) {
    { ptr } -> std::convertible_to<void*>;
};
#endif //SDLCONCEPTS_HPP
