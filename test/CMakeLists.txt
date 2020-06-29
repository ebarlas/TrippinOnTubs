# automatically enable catch2 to generate ctest targets
include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)

include_directories(../src/)
add_library(catch_main STATIC catch_main.cpp)
target_link_libraries(catch_main PUBLIC CONAN_PKG::catch2)

add_executable(engine_tests engine/engine_tests.cpp engine/object_tests.cpp engine/queue_tests.cpp engine/sides_tests.cpp engine/rect_tests.cpp engine/vector_tests.cpp engine/rect_tests.cpp)
target_link_libraries(engine_tests PRIVATE Engine catch_main)
catch_discover_tests(
        engine_tests
        TEST_PREFIX
        "unittests."
        EXTRA_ARGS
        -s
        --reporter=xml
        --out=tests.xml)

add_executable(sprite_tests sprite/sprite_metadata_tests.cpp sprite/sprite_sheet_tests.cpp)
target_link_libraries(sprite_tests PRIVATE Sprite catch_main)
catch_discover_tests(
        sprite_tests
        TEST_PREFIX
        "unittests."
        EXTRA_ARGS
        -s
        --reporter=xml
        --out=tests.xml)