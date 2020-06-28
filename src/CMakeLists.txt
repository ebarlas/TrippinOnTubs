include_directories(./)

add_library(Engine engine/Object.cpp engine/Physics.cpp engine/PlasmaPhysics.cpp engine/Sides.cpp engine/Engine.cpp)

add_library(Graphics graphics/FontRenderer.cpp graphics/RenderableObject.cpp)
target_link_libraries(Graphics CONAN_PKG::sdl2 CONAN_PKG::sdl2_ttf)

add_executable(Trippin main.cpp)
target_link_libraries(Trippin CONAN_PKG::nlohmann_json)

add_library(GameLoop gameloop.cpp)
target_link_libraries(GameLoop Graphics Engine)

add_executable(enclosed enclosed.cpp)
target_link_libraries(enclosed GameLoop Graphics Engine)

add_executable(arena arena.cpp)
target_link_libraries(arena GameLoop Graphics Engine)