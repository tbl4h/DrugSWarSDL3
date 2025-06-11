cmake_minimum_required(VERSION 3.31)
project(DrugSWarSDL3)

set(CMAKE_CXX_STANDARD 23)

# Włącz FetchContent
include(FetchContent)

# Włącz IPO/LTO jeśli jest dostępne
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)


# Ustaw katalog dla zewnętrznych bibliotek
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/external)

# Zapobiegaj konfliktom z cache
set(FETCHCONTENT_QUIET ON)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)

# Globalnie wyłącz wszystkie testy
set(BUILD_TESTING OFF CACHE BOOL "Disable testing" FORCE)
set(BUILD_TESTS OFF CACHE BOOL "Disable tests" FORCE)

# Sprawdź czy biblioteki już istnieją lokalnie
if (EXISTS ${CMAKE_SOURCE_DIR}/external/SDL3 AND EXISTS ${CMAKE_SOURCE_DIR}/external/SDL3/CMakeLists.txt)
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL3)
else ()
    FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG main
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/SDL3
    )
    FetchContent_MakeAvailable(SDL3)
endif ()

if (EXISTS ${CMAKE_SOURCE_DIR}/external/SDL3_image AND EXISTS ${CMAKE_SOURCE_DIR}/external/SDL3_image/CMakeLists.txt)
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/SDL3_image)
else ()
    FetchContent_Declare(
            SDL3_image
            GIT_REPOSITORY https://github.com/libsdl-org/SDL_image.git
            GIT_TAG main
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/SDL3_image
    )
    FetchContent_MakeAvailable(SDL3_image)
endif ()

# Specjalne ustawienia dla Tileson
set(TILESON_BUILD_TESTS OFF CACHE BOOL "Disable Tileson tests" FORCE)
set(TILESON_BUILD_EXAMPLES OFF CACHE BOOL "Disable Tileson examples" FORCE)

# Obsłuż Tileson jako bibliotekę header-only
if (EXISTS ${CMAKE_SOURCE_DIR}/external/tileson AND EXISTS ${CMAKE_SOURCE_DIR}/external/tileson/CMakeLists.txt)
    # Sprawdź czy Tileson tworzy target czy jest header-only
    if (EXISTS ${CMAKE_SOURCE_DIR}/external/tileson/include)
        # Header-only - utwórz interface library
        add_library(tileson INTERFACE)
        target_include_directories(tileson INTERFACE ${CMAKE_SOURCE_DIR}/external/tileson/include)
    else ()
        add_subdirectory(${CMAKE_SOURCE_DIR}/external/tileson)
    endif ()
else ()
    FetchContent_Declare(
            tileson
            GIT_REPOSITORY https://github.com/SSBMTonberry/tileson.git
            GIT_TAG v1.4.0
            SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/tileson
    )
    FetchContent_Populate(tileson)
    # Utwórz interface library dla header-only Tileson
    add_library(tileson INTERFACE)
    target_include_directories(tileson INTERFACE ${tileson_SOURCE_DIR}/include)
endif ()

add_executable(DrugSWarSDL3 main.cpp
        SDL_CPP/include/SDLResourcesConcepts.hpp
        SDL_CPP/include/SDLDeleters.hpp
        SDL_CPP/include/SDLResourcesAliases.hpp
        SDL_CPP/include/SDLFactoryFunctions.hpp
        SDL_CPP/include/SDLError.hpp
        SDL_CPP/include/SDLUtilityFunctions.hpp
        SDL_CPP/include/SDLArgumentsStructure.hpp
        SDL_CPP/src/SdlManager.cpp
        SDL_CPP/include/SdlManager.hpp
        SDL_CPP/include/SDLGameEngineStructures.hpp
)

# Linkuj biblioteki do wykonywalne
target_link_libraries(DrugSWarSDL3
        SDL3::SDL3
        SDL3_image::SDL3_image
        tileson
)

# Dodaj katalogi include
target_include_directories(DrugSWarSDL3 PRIVATE
        ${CMAKE_SOURCE_DIR}/external/SDL3/include
        ${CMAKE_SOURCE_DIR}/external/SDL3_image/include
        ${CMAKE_SOURCE_DIR}/external/tileson/include
)

if (ipo_supported)
    set_property(TARGET DrugSWarSDL3 PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    message(STATUS "IPO/LTO włączone dla DrugSWarSDL3")
else ()
    message(STATUS "IPO/LTO nie jest dostępne: ${ipo_error}")
endif ()
