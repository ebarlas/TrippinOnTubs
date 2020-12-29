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
        engine/SnapQueue.cpp
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

add_library(Game
        game/Game.cpp
        game/Map.cpp
        game/Configuration.cpp
        game/SpriteObject.cpp
        game/Ground.cpp
        game/WingedTub.cpp
        game/Mutex.cpp
        game/Lock.cpp
        game/Goggin.cpp
        game/JumpMeter.cpp
        game/Score.cpp
        game/SpiritClock.cpp
        game/Spirit.cpp)
target_link_libraries(Game Sprite)

add_library(Graphics
        graphics/FontRenderer.cpp
        graphics/RenderableObject.cpp)
target_include_directories(Graphics PUBLIC Engine ${SDL2_TTF_INCLUDE_DIR})
target_link_libraries(Graphics Engine ${SDL2_TTF_LIB})

add_library(GameLoop applications/gameloop.cpp)
target_link_libraries(GameLoop Graphics)

add_executable(enclosed applications/enclosed.cpp)
target_link_libraries(enclosed GameLoop Sprite)

add_executable(arena applications/arena.cpp)
target_link_libraries(arena GameLoop Sprite)

add_executable(frogs applications/frogs.cpp)
target_link_libraries(frogs GameLoop Sprite)

add_executable(mice applications/mice.cpp)
target_link_libraries(mice GameLoop Sprite)

add_executable(goggin applications/goggin.cpp)
target_link_libraries(goggin GameLoop Sprite)

add_executable(tgoggin applications/tgoggin.cpp)
target_link_libraries(tgoggin GameLoop Sprite)

add_executable(trippin applications/trippin.cpp)
target_link_libraries(trippin Game)

add_executable(collisiondebug applications/collisiondebug.cpp)
target_link_libraries(collisiondebug Engine)