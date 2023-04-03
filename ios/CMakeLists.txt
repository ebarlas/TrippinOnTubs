cmake_minimum_required(VERSION 3.24)
project(Trippin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_VERBOSE_MAKEFILE ON)

include(FetchContent)

FetchContent_Declare(
        SDL
        GIT_REPOSITORY https://github.com/libsdl-org/SDL
        GIT_TAG release-2.26.4)
FetchContent_MakeAvailable(SDL)

FetchContent_Declare(
        SDL_image
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_image
        GIT_TAG release-2.6.3)
set(SDL2IMAGE_BMP OFF)
set(SDL2IMAGE_GIF OFF)
set(SDL2IMAGE_JPG OFF)
set(SDL2IMAGE_INSTALL OFF)
set(BUILD_SHARED_LIBS FALSE)
FetchContent_MakeAvailable(SDL_image)

FetchContent_Declare(
        SDL_mixer
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_mixer
        GIT_TAG release-2.6.3)
set(SDL2MIXER_MIDI FALSE)
set(SDL2MIXER_OPUS FALSE)
set(SDL2MIXER_FLAC FALSE)
set(SDL2MIXER_MOD_MODPLUG FALSE)
set(SDL2MIXER_MIDI_FLUIDSYNTH FALSE)
FetchContent_MakeAvailable(SDL_mixer)

FetchContent_Declare(
        SDL_net
        GIT_REPOSITORY https://github.com/libsdl-org/SDL_net
        GIT_TAG release-2.2.0)
FetchContent_MakeAvailable(SDL_net)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

file(GLOB enginecpp src/engine/**.cpp)
file(GLOB spritecpp src/sprite/**.cpp)
file(GLOB netcpp src/net/**.cpp)
file(GLOB uicpp src/ui/**.cpp)
file(GLOB gamecpp src/game/**.cpp)
file(GLOB trippincpp src/trippin/**.cpp)

add_executable(trippin
        ${enginecpp}
        ${spritecpp}
        ${netcpp}
        ${uicpp}
        ${gamecpp}
        ${trippincpp}
)

target_include_directories(trippin PRIVATE src/)

target_link_libraries(
        trippin
        nlohmann_json::nlohmann_json
        SDL2::SDL2main
        SDL2::SDL2-static
        SDL2_net::SDL2_net-static
        SDL2_image::SDL2_image-static
        SDL2_mixer::SDL2_mixer-static
)
