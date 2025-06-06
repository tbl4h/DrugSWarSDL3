//
// Created by mic on 03.06.25.
//

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>
#include <memory>
#include <iostream>
#include <expected>
#include <string>
#include <string_view>
#include <concepts>
#include <array>
#include <chrono>
#include <format>
#include <optional>
#include <span>
#include <utility>
#include <vector>

#include "./SDL_CPP/include/SDLArgumentsStructure.hpp"
#include "./SDL_CPP/include/SDLError.hpp"
#include "./SDL_CPP/include/SdlManager.hpp"
#include "./SDL_CPP/include/SDLResourcesAliases.hpp"
#include "./SDL_CPP/include/SDLFactoryFunctions.hpp"
#include "./SDL_CPP/include/SDLDeleters.hpp"
#include "./SDL_CPP/include/SDLUtilityFunctions.hpp"
#include "./SDL_CPP/include/SDLResourcesConcepts.hpp"
#include "./SDL_CPP/include/SDLGameEngineStructures.hpp"

// C++20 Namespace organization
namespace SDL_App {
    class GameLoop {
    private:
        bool m_running{true};
        SDL_RendererSharedPtr renderer{};

    public:
        RenderLogicalPresentation render_logical_presentation{};

    public:
        GameLoop(SDL_RendererSharedPtr renderer) noexcept : renderer(std::move(renderer)) {
            std::cout << std::format("Logical size: {}x{}\n",
                                     render_logical_presentation.width,
                                     render_logical_presentation.height);

            int result = SDL_SetRenderLogicalPresentation(this->renderer.get(),
                                                          render_logical_presentation.width,
                                                          render_logical_presentation.height,
                                                          render_logical_presentation.renderer_logical_presentation
            );
            if (!result) {
                std::cerr << "❌ SDL_SetRenderLogicalPresentation() failed: " << SDL_GetError() << "\n";
            } else {
                std::cout << "✅ SDL_SetRenderLogicalPresentation() ok\n";
            }
            // Sprawdź rzeczywiste wartości
            int logical_w, logical_h;
            SDL_RendererLogicalPresentation mode;
            SDL_GetRenderLogicalPresentation(this->renderer.get(), &logical_w, &logical_h, &mode);
            std::cout << std::format("Actual logical: {}x{}, mode: {}\n", logical_w, logical_h, (int) mode);
        }

        [[nodiscard]] constexpr bool is_running() const noexcept { return m_running; }

        void stop() noexcept {
            m_running = false;
        }

        // C++20 chrono improvements
        [[nodiscard]] auto get_delta_time() noexcept {
        }

        [[nodiscard]] auto set_render_logical_presentation(SDL_Renderer *renderer, int width, int height,
                                                           SDL_RendererLogicalPresentation logical_presentation)
            noexcept {
            SDL_SetRenderLogicalPresentation(renderer, width, height, logical_presentation);
        }

        [[nodiscard]] auto set_render(SDL_RendererSharedPtr renderer) noexcept {
            this->renderer = std::move(renderer);
        }

        [[nodiscard]] auto get_render_logical_presentation() noexcept -> SDL_RendererSharedPtr { return renderer; }

        void process_events() noexcept {
            SDL_Event event{0};
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_EVENT_QUIT:
                        std::cout << "🚪 Otrzymano event quit\n";
                        stop();
                        break;
                    case SDL_EVENT_KEY_DOWN:
                        if (event.key.key == SDLK_ESCAPE) {
                            std::cout << "⎋ Escape naciśnięty\n";
                            stop();
                            break;
                        }
                    case SDL_EVENT_WINDOW_RESIZED:
                        render_logical_presentation.width = event.window.data1;
                        render_logical_presentation.height = event.window.data2;
                        handleEvent(event);
                        break;
                    default:
                        break;
                }
            }
        }
    };
} // namespace SDL_App

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
    using namespace std::chrono_literals;

    std::cout << "🚀 Uruchamianie Modern C++ SDL3\n";

    // C++20 Designated initializers - POPRAWKA: bez constexpr dla std::string
    WindowConfig window_config{
        .title = "Modern C++ SDL3",
        .width = 1024,
        .height = 768,
        .flags = SDL_WINDOW_RESIZABLE
    };

    RenderConfig render_config{
        .clear_color = {0, 0, 100, 0},
        .renderer_name = std::nullopt
    };
    SDLState sdl_state;
    // C++17 Structured bindings with C++23 std::expected
    auto sdl_result = initializeSDL();
    if (!sdl_result) [[unlikely]] {
        const auto error_msg = error_to_string(sdl_result.error());
        std::cerr << std::format("❌ {}\n", error_msg);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL Init Error",
                                 error_msg.c_str(), nullptr);
        return 1;
    }

    auto sdl = std::move(sdl_result.value());

    auto window_result = createWindow(window_config);
    if (!window_result) [[unlikely]] {
        const auto error_msg = error_to_string(window_result.error());
        std::cerr << std::format("❌ {}\n", error_msg);
        return 1;
    }

    auto window = std::move(window_result.value());
    auto renderer_result = createRenderer(window.get(), render_config);
    if (!renderer_result) [[unlikely]] {
        const auto error_msg = error_to_string(renderer_result.error());
        std::cerr << std::format("❌ {}\n", error_msg);
        return 1;
    }

    auto renderer = std::move(renderer_result.value());

    sdl_state.window = std::move(window);
    sdl_state.renderer = std::move(renderer);


    SDL_App::GameLoop game_loop(sdl_state.renderer);

    std::cout
            <<
            "✅ SDL3 zainicjalizowane pomyślnie\n";
    std::cout << "✅ Okno i renderer utworzone\n";
    std::cout << "🎮 Naciśnij ESC lub zamknij okno, aby zakończyć\n";


    auto idle_texture_resoult = createTexture(sdl_state.renderer.get(), idle_texture_path);
    if (!idle_texture_resoult) [[unlikely]] {
        const auto error_msg = error_to_string(idle_texture_resoult.error());
        std::cerr << std::format("❌ {}\n", error_msg);
        return 1;
    }
    auto idle_texture = std::move(idle_texture_resoult.value());
    SDL_SetTextureScaleMode(idle_texture.get(), SDL_SCALEMODE_NEAREST);
    // C++20 Game loop z namespace


    warm_up_cache(sdl_state.renderer.get());

    while (game_loop.is_running()) {
        game_loop.process_events();

        // Renderowanie
        performRender(sdl_state.renderer.get(), render_config.clear_color);
        renderTexture(sdl_state.renderer.get(), idle_texture.get(), 0, 0);
        SDL_RenderPresent(sdl_state.renderer.get());
    }

    std::cout << std::format("🎮 Gra zakończona.\n");

    return 0;
}
