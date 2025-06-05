//
// Created by mic on 03.06.25.
//

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <memory>
#include <iostream>
#include <expected>
#include <string>
#include <string_view>
#include <concepts>
//#include <ranges>
#include <array>
#include <chrono>
#include <format>
#include <optional>
#include <span>
#include <utility>
#include <vector>

// C++20 Concepts
template<typename T>
concept SDL_Resource = requires(T* ptr) {
    { ptr } -> std::convertible_to<void*>;
};

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

// C++17 Template aliases z C++20 concepts
template<SDL_Resource T>
using SDL_UniquePtr = std::unique_ptr<T, SDL_CustomDeleter<T>>;

using SDL_WindowPtr = SDL_UniquePtr<SDL_Window>;
using SDL_RendererPtr = SDL_UniquePtr<SDL_Renderer>;
using SDL_TexturePtr = SDL_UniquePtr<SDL_Texture>;
using SDL_SurfacePtr = SDL_UniquePtr<SDL_Surface>;

// C++20 Strong typing z enum class
enum class [[nodiscard]] SDL_Error : std::uint8_t {
    InitFailed = 1,
    WindowCreationFailed,
    RendererCreationFailed,
    RenderingFailed
};

// C++20 constexpr
constexpr std::string_view get_error_prefix(SDL_Error error) noexcept {
    switch (error) {
        case SDL_Error::InitFailed: return "SDL initialization failed";
        case SDL_Error::WindowCreationFailed: return "Window creation failed";
        case SDL_Error::RendererCreationFailed: return "Renderer creation failed";
        case SDL_Error::RenderingFailed: return "Rendering failed";
    }
    return "Unknown error";
}

// POPRAWKA: Bezpieczne formatowanie
[[nodiscard]] std::string error_to_string(SDL_Error error) {
    try {
        auto prefix = get_error_prefix(error);
        auto sdl_error = SDL_GetError();
        return std::format("{}: {}", prefix, sdl_error ? sdl_error : "No error");
    } catch (...) {
        return "Error formatting failed";
    }
}

// C++20 Designated initializers structure
struct WindowConfig {
    std::string title{"SDL3 Window"};  // POPRAWKA: std::string zamiast string_view
    int width{800};
    int height{600};
    Uint32 flags{0};
};

struct RenderConfig {
    std::array<Uint8, 4> clear_color{64, 128, 255, 255};
    std::optional<std::string> renderer_name{std::nullopt};  // POPRAWKA: std::string
};

// Bezpieczna inicjalizacja SDL
[[nodiscard]] std::expected<std::unique_ptr<SDL_Manager>, SDL_Error> 
initializeSDL() noexcept {
    try {
        auto sdl = std::make_unique<SDL_Manager>();
        if (!sdl->initialized()) [[unlikely]] {
            return std::unexpected(SDL_Error::InitFailed);
        }
        return sdl;
    } catch (...) {
        return std::unexpected(SDL_Error::InitFailed);
    }
}

// POPRAWKA: Bezpieczne tworzenie okna
[[nodiscard]] auto createWindow(const WindowConfig& config) noexcept 
    -> std::expected<SDL_WindowPtr, SDL_Error> {
    
    try {
        auto* window = SDL_CreateWindow(
            config.title.c_str(),  // POPRAWKA: c_str() zamiast data()
            config.width,
            config.height,
            config.flags
        );
        
        if (!window) [[unlikely]] {
            std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
            return std::unexpected(SDL_Error::WindowCreationFailed);
        }
        
        std::cout << std::format("✅ Okno utworzone: {}x{}\n", config.width, config.height);
        return SDL_WindowPtr{window};
    } catch (...) {
        return std::unexpected(SDL_Error::WindowCreationFailed);
    }
}

[[nodiscard]] auto createRenderer(SDL_Window* window, const RenderConfig& config) noexcept
    -> std::expected<SDL_RendererPtr, SDL_Error> {
    
    try {
        const char* name = config.renderer_name.has_value() 
            ? config.renderer_name->c_str()  // POPRAWKA: c_str()
            : nullptr;
            
        auto* renderer = SDL_CreateRenderer(window, name);
        if (!renderer) [[unlikely]] {
            std::cerr << "Renderer creation failed: " << SDL_GetError() << "\n";
            return std::unexpected(SDL_Error::RendererCreationFailed);
        }
        
        std::cout << "✅ Renderer utworzony\n";
        return SDL_RendererPtr{renderer};
    } catch (...) {
        return std::unexpected(SDL_Error::RendererCreationFailed);
    }
}

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

// C++20 Namespace organization
namespace SDL_App {
    
    class GameLoop {
    private:
        bool m_running{true};
        std::chrono::steady_clock::time_point m_last_frame;
        std::uint64_t m_frame_count{0};
        
    public:
        GameLoop() noexcept : m_last_frame{std::chrono::steady_clock::now()} {}
        
        [[nodiscard]] constexpr bool is_running() const noexcept { return m_running; }
        
        void stop() noexcept { 
            std::cout << std::format("🛑 Zatrzymywanie po {} klatkach\n", m_frame_count);
            m_running = false; 
        }
        
        // C++20 chrono improvements
        [[nodiscard]] auto get_delta_time() noexcept {
            auto now = std::chrono::steady_clock::now();
            auto delta = now - m_last_frame;
            m_last_frame = now;
            ++m_frame_count;
            return std::chrono::duration_cast<std::chrono::milliseconds>(delta);
        }
        
        void process_events() noexcept {
            SDL_Event event{};
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
                        handleEvent(event);
                        break;
                    default:
                        break;
                }
            }
        }
        
        [[nodiscard]] std::uint64_t frame_count() const noexcept { return m_frame_count; }
    };
    
} // namespace SDL_App

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
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
        .clear_color = {32, 64, 128, 255},
        .renderer_name = std::nullopt
    };
    
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
    
    // POPRAWKA: Uproszczone wyświetlanie bez ranges
    std::cout << "📋 Dostępne eventy:\n";
    for (const auto& event_info : getEventNames()) {
        std::cout << std::format("   {}\n", event_info);
    }
    
    std::cout << "✅ SDL3 zainicjalizowane pomyślnie\n";
    std::cout << "✅ Okno i renderer utworzone\n";
    std::cout << "🎮 Naciśnij ESC lub zamknij okno aby zakończyć\n";
    
    // C++20 Game loop z namespace
    SDL_App::GameLoop game_loop;
    
    while (game_loop.is_running()) {
        game_loop.process_events();
        
        // Renderowanie
        performRender(renderer.get(), render_config.clear_color);
        
        // C++20 chrono literals z frame limiting
        auto delta = game_loop.get_delta_time();
        if (delta < 16ms) {
            SDL_Delay(static_cast<Uint32>((16ms - delta).count()));
        }
        
        // Status co 1000 klatek
        if (game_loop.frame_count() % 1000 == 0 && game_loop.frame_count() > 0) {
            std::cout << std::format("📊 Klatka: {}\n", game_loop.frame_count());
        }
    }
    
    std::cout << std::format("🎮 Gra zakończona po {} klatkach\n", game_loop.frame_count());
    
    return 0;
}