//
// Created by mic on 04.06.25.
//

#ifndef SDLRESOURCESALIASES_HPP
#define SDLRESOURCESALIASES_HPP

#include <memory>
#include "SDLResourcesConcepts.hpp"
#include "SDLDeleters.hpp"

// C++17 Template aliases z C++20 concepts
template<SDL_Resource T>
using SDL_UniquePtr = std::unique_ptr<T, SDL_CustomDeleter<T> >;

template<SDL_Resource T>
using SDL_SharedPtr = std::shared_ptr<T>;


using SDL_WindowPtr = SDL_UniquePtr<SDL_Window>;
using SDL_RendererPtr = SDL_UniquePtr<SDL_Renderer>;
using SDL_TexturePtr = SDL_UniquePtr<SDL_Texture>;
using SDL_SurfacePtr = SDL_UniquePtr<SDL_Surface>;

using SDL_WindowSharedPtr = SDL_SharedPtr<SDL_Window>;
using SDL_RendererSharedPtr = SDL_SharedPtr<SDL_Renderer>;
using SDL_TextureSharedPtr = SDL_SharedPtr<SDL_Texture>;
using SDL_SurfaceSharedPtr = SDL_SharedPtr<SDL_Surface>;

#endif //SDLRESOURCESALIASES_HPP
