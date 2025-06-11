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

namespace SDL_App {
    class SDLInitializer {
    private:
        SDL_StateSharedPtr m_sdl_state{};
        std::unique_ptr<SDL_Manager> m_sdl_manager{};
        bool m_initialized{false};

    public:
        SDLInitializer() : m_sdl_state(std::make_shared<SDLState>()) {
            m_sdl_state->width = 1024;
            m_sdl_state->height = 768;
            m_sdl_state->logH = 480;
            m_sdl_state->logW = 640;
        }

        [[nodiscard]] auto initialize(const WindowConfig &window_config, const RenderConfig &render_config)
            -> std::expected<void, SDLError> {
            // Initialize SDL
            auto sdl_result = initializeSDL();
            if (!sdl_result) {
                return std::unexpected(sdl_result.error());
            }
            m_sdl_manager = std::move(sdl_result.value());

            // Create window
            auto window_result = createWindow(window_config);
            if (!window_result) {
                return std::unexpected(window_result.error());
            }

            // Create renderer
            auto renderer_result = createRenderer(window_result.value().get(), render_config);
            if (!renderer_result) {
                return std::unexpected(renderer_result.error());
            }

            // Store resources in shared state
            m_sdl_state->window = std::move(window_result.value());
            m_sdl_state->renderer = std::move(renderer_result.value());
            m_initialized = true;

            std::cout << "✅ SDL3 zainicjalizowane pomyślnie\n";
            std::cout << "✅ Okno i renderer utworzone\n";

            return {};
        }

        [[nodiscard]] auto get_sdl_state() const noexcept -> std::shared_ptr<SDLState> {
            return m_sdl_state;
        }

        [[nodiscard]] constexpr bool is_initialized() const noexcept {
            return m_initialized;
        }
    };

    class GameLoop {
    private:
        bool m_running{true};
        SDL_StateSharedPtr m_sdl_state{};
        SDL_TextureSharedPtr m_idle_texture{};
        float m_delta_time{0};
        Uint64 m_last_frame_time{0};
        const bool *m_keys;
        float m_player_x{150};
        float m_floor{0};
        const float m_sprite_size{32};
        float m_move_amount{0};
        bool m_flip_horizontal{false};

    public:

    public:
        explicit GameLoop(std::shared_ptr<SDLState> sdl_state) noexcept
            : m_sdl_state((sdl_state)) {
        }

        [[nodiscard]] auto initialize_resources() -> std::expected<void, SDLError> {
            if (!m_sdl_state || !m_sdl_state->renderer) {
                return std::unexpected(SDLError::SDLStateFailed);
            }

            // Load idle texture
            auto idle_texture_result = createTexture(m_sdl_state->renderer.get(), idle_texture_path);
            if (!idle_texture_result) {
                return std::unexpected(idle_texture_result.error());
            }

            m_idle_texture = std::move(idle_texture_result.value());
            SDL_SetTextureScaleMode(m_idle_texture.get(), SDL_SCALEMODE_NEAREST);

            // Setup logical presentation
            int result = SDL_SetRenderLogicalPresentation(
                m_sdl_state->renderer.get(),
                m_sdl_state->logW,
                m_sdl_state->logH,
                SDL_LOGICAL_PRESENTATION_LETTERBOX
            );

            if (!result) {
                std::cerr << "❌ SDL_SetRenderLogicalPresentation() failed: " << SDL_GetError() << "\n";
                return std::unexpected(SDLError::RendererCreationFailed);
            }

            std::cout << "✅ SDL_SetRenderLogicalPresentation() ok\n";
            std::cout << std::format("Logical size: {}x{}\n",
                                     m_sdl_state->width,
                                     m_sdl_state->height);

            m_keys = SDL_GetKeyboardState(nullptr);
            m_floor = m_sdl_state->logH;
            // Warm up cache
            warm_up_cache(m_sdl_state->renderer.get());

            return {};
        }

        [[nodiscard]] constexpr bool is_running() const noexcept {
            return m_running;
        }

        void stop() noexcept {
            m_running = false;
        }

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
                        }
                        break;
                    case SDL_EVENT_WINDOW_RESIZED:
                        m_sdl_state->width = event.window.data1;
                        m_sdl_state->height = event.window.data2;
                        break;
                    default:
                        break;
                }
            }
        }

        void render(const RenderConfig &render_config) noexcept {
            if (!m_sdl_state || !m_sdl_state->renderer) {
                return;
            }

            performRender(m_sdl_state->renderer.get(), render_config.clear_color);
            SDL_FRect src_rect{0, 0, m_sprite_size, m_sprite_size};
            SDL_FRect dst_rect{m_player_x, m_floor - m_sprite_size, m_sprite_size, m_sprite_size};
            //SDL_RenderTexture(m_sdl_state->renderer.get(), m_idle_texture.get(), &src_rect, &dst_rect);
            SDL_RenderTextureRotated(m_sdl_state->renderer.get(), m_idle_texture.get(), &src_rect, &dst_rect, 0,
                                     nullptr, (m_flip_horizontal) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE);
            SDL_RenderPresent(m_sdl_state->renderer.get());
        }

        [[nodiscard]] auto get_sdl_state() const noexcept -> std::shared_ptr<SDLState> {
            return m_sdl_state;
        }

        void move_player(float delta_time) noexcept {
            if (m_keys[SDL_SCANCODE_A]) {
                m_move_amount += -5.0f;
                m_flip_horizontal = true;
            } else if (m_keys[SDL_SCANCODE_D]) {
                m_move_amount += 5.0f;
                m_flip_horizontal = false;
            } else {
                m_move_amount = 0.0f;
            }
            m_player_x += m_move_amount * delta_time;
        }

        void update_delta_time() noexcept {
            Uint64 current_time = SDL_GetTicks();
            m_delta_time = (current_time - m_last_frame_time) / 1000.0f;

            // Optional: Cap delta time aby uniknąć "spiral of death"
            constexpr float max_delta = 1.0f / 32.0f; // Max 20 FPS
            if (m_delta_time > max_delta) {
                m_delta_time = max_delta;
            }

            m_last_frame_time = current_time;
        }

        float get_delta_time() const noexcept {
            return m_delta_time;
        }
    };
} // namespace SDL_App

int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[]) {
    using namespace std::chrono_literals;

    std::cout << "🚀 Uruchamianie Modern C++ SDL3\n";

    // Configuration
    WindowConfig window_config{
        .title = "Modern C++ SDL3",
        .width = 1024,
        .height = 768,
        .flags = SDL_WINDOW_RESIZABLE
    };

    RenderConfig render_config{
        .clear_color = {0, 0, 30, 0},
        .renderer_name = std::nullopt
    };

    // Initialize SDL
    SDL_App::SDLInitializer sdl_initializer;
    auto init_result = sdl_initializer.initialize(window_config, render_config);
    if (!init_result) [[unlikely]] {
        const auto error_msg = error_to_string(init_result.error());
        std::cerr << std::format("❌ {}\n", error_msg);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "SDL Init Error",
                                 error_msg.c_str(), nullptr);
        return 1;
    }

    // Create game loop with shared SDL state
    SDL_App::GameLoop game_loop(sdl_initializer.get_sdl_state());

    // Initialize game resources
    auto resources_result = game_loop.initialize_resources();
    if (!resources_result) [[unlikely]] {
        const auto error_msg = error_to_string(resources_result.error());
        std::cerr << std::format("❌ {}\n", error_msg);
        return 1;
    }

    std::cout << "🎮 Naciśnij ESC lub zamknij okno, aby zakończyć\n";

    // Main game loop
    while (game_loop.is_running()) {
        game_loop.update_delta_time();
        game_loop.process_events();
        game_loop.render(render_config);
        game_loop.move_player(game_loop.get_delta_time());
    }

    std::cout << std::format("🎮 Gra zakończona.\n");
    return 0;
}
