add_library(Sides engine/sides.cpp)
add_library(PlasmaPhysics engine/plasmaphysics.cpp)
add_library(Physics engine/physics.cpp)

add_executable(Trippin main.cpp)
target_link_libraries(Trippin CONAN_PKG::nlohmann_json)

add_executable(enclosed enclosed.cpp)
target_link_libraries(enclosed Sides PlasmaPhysics Physics CONAN_PKG::sdl2 CONAN_PKG::sdl2_ttf)