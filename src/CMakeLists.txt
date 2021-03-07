find_package(SDL2)

add_library(Engine
        engine/PlasmaPhysics.cpp
        engine/PlatformCollision.cpp
        engine/ObjectCollision.cpp
        engine/AbsorbentCollision.cpp
        engine/ReflectiveCollision.cpp
        engine/ElasticCollision1D.cpp
        engine/ElasticCollision2D.cpp
        engine/InelasticCollision.cpp
        engine/Listener.cpp
        engine/Object.cpp
        engine/Sides.cpp
        engine/Engine.cpp
        engine/Timer.cpp
        engine/Clock.cpp)
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Engine SDL2::SDL2)

add_library(Sprite
        sprite/Scale.cpp
        sprite/SpriteMetadata.cpp
        sprite/SpriteSheet.cpp
        sprite/Sprite.cpp
        sprite/SpriteManager.cpp
        sprite/Files.cpp
        sprite/Camera.cpp)
target_include_directories(Sprite PUBLIC ${NLOHMANN_JSON_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR})
target_link_libraries(Sprite Engine ${SDL2_IMAGE_LIB})

add_library(Lock
        lock/Lock.cpp
        lock/Mutex.cpp)
target_link_libraries(Lock SDL2::SDL2)

add_library(Graphics
        graphics/FontRenderer.cpp
        graphics/RenderableObject.cpp)
target_include_directories(Graphics PUBLIC Engine ${SDL2_TTF_INCLUDE_DIR})
target_link_libraries(Graphics Engine ${SDL2_TTF_LIB})

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
        game/Game.cpp
        game/Map.cpp
        game/Configuration.cpp
        game/SpriteObject.cpp
        game/Ground.cpp
        game/WingedTub.cpp
        game/Goggin.cpp
        game/JumpMeter.cpp
        game/Score.cpp
        game/SpiritClock.cpp
        game/Spirit.cpp
        game/PacingObject.cpp
        game/RunningClock.cpp
        game/Bird.cpp
        game/Ball.cpp
        game/Activation.cpp
        game/Layer.cpp
        game/Level.cpp
        game/AutoPlay.cpp
        game/TitleOverlay.cpp
        game/MenuOverlay.cpp
        game/SoundManager.cpp)
target_include_directories(Game PUBLIC ${SDL2_MIXER_INCLUDE_DIR})
target_link_libraries(Game Sprite Lock ${SDL2_MIXER_LIB})

add_executable(trippin game/trippin.cpp)
target_link_libraries(trippin Game)

add_executable(collisiondebug demo/collisiondebug.cpp)
target_link_libraries(collisiondebug Engine)