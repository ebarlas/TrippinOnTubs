include_directories(./)

add_library(Engine engine/object.cpp engine/physics.cpp engine/plasmaphysics.cpp engine/sides.cpp)

add_library(Graphics graphics/FontRenderer.cpp graphics/RenderableObject.cpp)
target_link_libraries(Graphics CONAN_PKG::sdl2 CONAN_PKG::sdl2_ttf)

add_executable(Trippin main.cpp)
target_link_libraries(Trippin CONAN_PKG::nlohmann_json)

add_library(GameLoop gameloop.cpp)
target_link_libraries(GameLoop Graphics Engine)

add_executable(enclosed enclosed.cpp)
target_link_libraries(enclosed GameLoop Graphics Engine)