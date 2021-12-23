find_package(SDL2 REQUIRED)
find_package(nlohmann_json REQUIRED)

find_package(PkgConfig REQUIRED)

pkg_check_modules(SDL2_MIXER REQUIRED IMPORTED_TARGET sdl2_mixer)
pkg_check_modules(SDL2_IMAGE REQUIRED IMPORTED_TARGET sdl2_image)
pkg_check_modules(SDL2_TTF REQUIRED IMPORTED_TARGET sdl2_ttf)
pkg_check_modules(SDL2_NET REQUIRED IMPORTED_TARGET sdl2_net)

add_library(Engine
        engine/PlasmaPhysics.cpp
        engine/Listener.cpp
        engine/Object.cpp
        engine/Sides.cpp
        engine/Engine.cpp
        engine/Timer.cpp)
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Engine SDL2::SDL2)

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
        ui/LevelOverlay.cpp)
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

add_executable(goggin demo/goggin.cpp)
target_link_libraries(goggin GameLoop Sprite)

add_executable(tgoggin demo/tgoggin.cpp)
target_link_libraries(tgoggin GameLoop Sprite)

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
        game/SoundManager.cpp)
target_link_libraries(Game Sprite Net Ui PkgConfig::SDL2_MIXER)

add_executable(trippin game/trippin.cpp)
target_link_libraries(trippin Game)

add_executable(collisiondebug demo/collisiondebug.cpp)
target_link_libraries(collisiondebug Engine)