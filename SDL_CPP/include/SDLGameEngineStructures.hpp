//
// Created by mic on 06.06.25.
//

#ifndef SDLGAMEENGINESTRUCTURES_HPP
#define SDLGAMEENGINESTRUCTURES_HPP

#include "SDLResourcesAliases.hpp"

const std::string project_root = std::filesystem::current_path().parent_path().string();
const std::string idle_texture_path = project_root + "/Data/idle.png";


struct alignas (64) SDLState {
    SDL_WindowSharedPtr window{};
    SDL_RendererSharedPtr renderer{};
};

#endif //SDL_GAMEENGINESTRUCTURES_HPP
