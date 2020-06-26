cmake_minimum_required(VERSION 3.15)
project(Trippin)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_STANDARD 17)

include(build/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_subdirectory(src)
add_subdirectory(test)