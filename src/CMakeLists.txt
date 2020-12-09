include_directories(./)

add_library(Engine
        engine/PlasmaPhysics.cpp
        engine/PlatformCollision.cpp
        engine/ObjectCollision.cpp
        engine/AbsorbentCollision.cpp
        engine/ReflectiveCollision.cpp
        engine/ElasticCollision1D.cpp
        engine/ElasticCollision2D.cpp
        engine/InelasticCollision.cpp
        engine/Object.cpp
        engine/Sides.cpp
        engine/Engine.cpp
        engine/SnapQueue.cpp
        engine/Clock.cpp)
target_link_libraries(Engine CONAN_PKG::sdl2)

add_library(Sprite
        sprite/Scale.cpp
        sprite/SpriteMetadata.cpp
        sprite/SpriteSheet.cpp
        sprite/Sprite.cpp
        sprite/SpriteManager.cpp
        sprite/Files.cpp
        sprite/Camera.cpp)
target_link_libraries(Sprite CONAN_PKG::nlohmann_json CONAN_PKG::sdl2 CONAN_PKG::sdl2_image)

add_library(Game
        game/Game.cpp
        game/Map.cpp
        game/Configuration.cpp
        game/SpriteObject.cpp
        game/Ground.cpp
        game/WingedTub.cpp
        game/Lock.cpp
        game/Goggin.cpp
        game/Spirit.cpp)
target_link_libraries(Game CONAN_PKG::nlohmann_json CONAN_PKG::sdl2 CONAN_PKG::sdl2_image)

add_library(Graphics graphics/FontRenderer.cpp graphics/RenderableObject.cpp)
target_link_libraries(Graphics Engine CONAN_PKG::sdl2_ttf)

add_library(GameLoop applications/gameloop.cpp)
target_link_libraries(GameLoop Graphics Engine)

add_executable(enclosed applications/enclosed.cpp)
target_link_libraries(enclosed GameLoop Graphics Engine)

add_executable(arena applications/arena.cpp)
target_link_libraries(arena GameLoop Graphics Engine)

add_executable(frogs applications/frogs.cpp)
target_link_libraries(frogs GameLoop Graphics Engine)

add_executable(mice applications/mice.cpp)
target_link_libraries(mice GameLoop Graphics Engine)

add_executable(goggin applications/goggin.cpp)
target_link_libraries(goggin GameLoop Sprite Engine)

add_executable(tgoggin applications/tgoggin.cpp)
target_link_libraries(tgoggin GameLoop Sprite Engine)

add_executable(trippin applications/trippin.cpp)
target_link_libraries(trippin Sprite Engine Game)

add_executable(collisiondebug applications/collisiondebug.cpp)
target_link_libraries(collisiondebug Engine)