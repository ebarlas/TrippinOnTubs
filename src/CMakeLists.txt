include_directories(./)

add_library(Engine
        engine/Object.cpp
        engine/Physics.cpp
        engine/PlasmaPhysics.cpp
        engine/Sides.cpp
        engine/Engine.cpp
        engine/Partition.cpp
        engine/Grid.cpp
        engine/SnapQueue.cpp)
target_link_libraries(Engine CONAN_PKG::sdl2)

add_library(Sdl sdl/Lib.cpp sdl/Window.cpp sdl/Renderer.cpp sdl/Surface.cpp sdl/Texture.cpp)
target_link_libraries(Sdl CONAN_PKG::sdl2 CONAN_PKG::sdl2_image)

add_library(Sprite
        sprite/SpriteMetadata.cpp
        sprite/SpriteSheet.cpp
        sprite/Sprite.cpp
        sprite/SpriteManager.cpp
        sprite/Files.cpp
        sprite/Camera.cpp)
target_link_libraries(Sprite Sdl CONAN_PKG::nlohmann_json CONAN_PKG::sdl2 CONAN_PKG::sdl2_image)

add_library(Graphics graphics/FontRenderer.cpp graphics/RenderableObject.cpp)
target_link_libraries(Graphics CONAN_PKG::sdl2 CONAN_PKG::sdl2_ttf)

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

add_executable(trippin applications/trippin.cpp applications/Game.cpp)
target_link_libraries(trippin Sprite Engine)