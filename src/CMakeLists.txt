find_package(SDL2 REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(PkgConfig REQUIRED)

pkg_search_module(SDL2_MIXER REQUIRED IMPORTED_TARGET sdl2_mixer SDL2_mixer)
pkg_search_module(SDL2_IMAGE REQUIRED IMPORTED_TARGET sdl2_image SDL2_image)
pkg_search_module(SDL2_TTF REQUIRED IMPORTED_TARGET sdl2_ttf SDL2_ttf)
pkg_search_module(SDL2_NET REQUIRED IMPORTED_TARGET sdl2_net SDL2_net)

# https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md
set(CLANG_WARNINGS
    -Wall
    -Wextra # reasonable and standard
    -Wshadow # warn the user if a variable declaration shadows one from a parent context
    -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps
    # catch hard to track down memory errors
    -Wold-style-cast # warn for c-style casts
    -Wcast-align # warn for potential performance problem casts
    -Wunused # warn on anything being unused
    -Woverloaded-virtual # warn if you overload (not override) a virtual function
    -Wpedantic # warn if non-standard C++ is used
    -Wconversion # warn on type conversions that may lose data
    -Wsign-conversion # warn on sign conversions
    -Wnull-dereference # warn if a null dereference is detected
    -Wdouble-promotion # warn if float is implicit promoted to double
    -Wformat=2 # warn on security issues around functions that format output (ie printf)
    -Wimplicit-fallthrough # warn on statements that fallthrough without an explicit annotation
)

set(GCC_WARNINGS
    ${CLANG_WARNINGS}
    -Wmisleading-indentation # warn if indentation implies blocks where blocks do not exist
    -Wduplicated-cond # warn if if / else chain has duplicated conditions
    -Wduplicated-branches # warn if if / else branches have duplicated code
    -Wlogical-op # warn about logical operations being used where bitwise were probably wanted
    -Wuseless-cast # warn if you perform a cast to the same type
)

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    set(PROJECT_WARNINGS ${CLANG_WARNINGS})
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(PROJECT_WARNINGS ${GCC_WARNINGS})
else()
    message(AUTHOR_WARNING "No compiler warnings set for CXX compiler: '${CMAKE_CXX_COMPILER_ID}'")
endif()

add_library(Engine
        engine/PlasmaPhysics.cpp
        engine/Listener.cpp
        engine/Object.cpp
        engine/Sides.cpp
        engine/Engine.cpp
        engine/Clock.cpp
        engine/Timer.cpp)
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Engine SDL2::SDL2)
target_compile_options(Engine INTERFACE ${PROJECT_WARNINGS})

add_library(Sprite
        sprite/Scale.cpp
        sprite/SpriteMetadata.cpp
        sprite/SpriteSheet.cpp
        sprite/Sprite.cpp
        sprite/SpriteManager.cpp
        sprite/SpriteLoader.cpp
        sprite/SpriteLoadTask.cpp
        sprite/Files.cpp
        sprite/Camera.cpp
        sprite/RenderClock.cpp)
target_include_directories(Sprite PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Sprite Engine nlohmann_json::nlohmann_json PkgConfig::SDL2_IMAGE)

add_library(Graphics
        demo/FontRenderer.cpp
        demo/RenderableObject.cpp)
target_include_directories(Graphics PUBLIC Engine)
target_link_libraries(Graphics Engine PkgConfig::SDL2_TTF)

add_library(Net
        net/Tcp.cpp
        net/Transport.cpp
        net/DbSynchronizer.cpp
        net/Logger.cpp
        net/StagingArea.cpp)
target_include_directories(Net PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Net PkgConfig::SDL2_NET nlohmann_json::nlohmann_json)

add_library(Ui
        ui/ScoreBoard.cpp
        ui/ScrollingScoreBoard.cpp
        ui/Interpolator.cpp
        ui/TitleMenu.cpp
        ui/TitleOverlay.cpp
        ui/EndMenu.cpp
        ui/NameForm.cpp
        ui/ScoreMenu.cpp
        ui/LevelOverlay.cpp
        ui/ExitOverlay.cpp)
target_link_libraries(Ui Net)

add_library(GameLoop demo/gameloop.cpp)
target_link_libraries(GameLoop Graphics)

add_executable(enclosed demo/enclosed.cpp)
target_link_libraries(enclosed GameLoop Sprite)

add_executable(arena demo/arena.cpp)
target_link_libraries(arena GameLoop Sprite)

add_executable(frogs demo/frogs.cpp)
target_link_libraries(frogs GameLoop Sprite)

add_executable(mice demo/mice.cpp)
target_link_libraries(mice GameLoop Sprite)

add_library(Game
        game/SdlSystem.cpp
        game/Game.cpp
        game/UserInput.cpp
        game/Map.cpp
        game/Configuration.cpp
        game/SpriteObject.cpp
        game/Ground.cpp
        game/WingedTub.cpp
        game/Goggin.cpp
        game/GogginRenderer.cpp
        game/JumpMeter.cpp
        game/LifeMeter.cpp
        game/ScoreTicker.cpp
        game/SpiritClock.cpp
        game/Spirit.cpp
        game/GameObject.cpp
        game/TrainingProgram.cpp
        game/RunningClock.cpp
        game/Activation.cpp
        game/Layer.cpp
        game/Level.cpp
        game/AutoPlay.cpp
        game/Shake.cpp
        game/SceneBuilder.cpp
        game/ComboManager.cpp
        game/PointCloudManager.cpp
        game/GroupManager.cpp
        game/SoundManager.cpp)
target_link_libraries(Game Sprite Net Ui PkgConfig::SDL2_MIXER)

add_executable(trippin game/trippin.cpp)
target_link_libraries(trippin Game)

add_executable(collisiondebug demo/collisiondebug.cpp)
target_link_libraries(collisiondebug Engine)