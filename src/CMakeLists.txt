add_executable(Trippin main.cpp)
target_link_libraries(Trippin CONAN_PKG::nlohmann_json)

add_executable(enclosed enclosed.cpp)
target_link_libraries(enclosed CONAN_PKG::sdl2 CONAN_PKG::sdl2_ttf)

