# automatically enable catch2 to generate ctest targets
include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)

include_directories(../src/)
add_library(catch_main STATIC catch_main.cpp)
target_link_libraries(catch_main PUBLIC CONAN_PKG::catch2)

add_executable(vector_tests engine/vector_tests.cpp)
target_link_libraries(vector_tests PRIVATE catch_main)
catch_discover_tests(
        vector_tests
        TEST_PREFIX
        "unittests."
        EXTRA_ARGS
        -s
        --reporter=xml
        --out=tests.xml)

add_executable(rect_tests engine/rect_tests.cpp)
target_link_libraries(rect_tests PRIVATE catch_main)
catch_discover_tests(
        rect_tests
        TEST_PREFIX
        "unittests."
        EXTRA_ARGS
        -s
        --reporter=xml
        --out=tests.xml)

add_executable(sides_tests engine/sides_tests.cpp)
target_link_libraries(sides_tests PRIVATE Sides catch_main)
catch_discover_tests(
        sides_tests
        TEST_PREFIX
        "unittests."
        EXTRA_ARGS
        -s
        --reporter=xml
        --out=tests.xml)

add_executable(queue_tests engine/queue_tests.cpp)
target_link_libraries(queue_tests PRIVATE catch_main)
catch_discover_tests(
        queue_tests
        TEST_PREFIX
        "unittests."
        EXTRA_ARGS
        -s
        --reporter=xml
        --out=tests.xml)

add_executable(object_tests engine/object_tests.cpp)
target_link_libraries(object_tests PRIVATE Engine catch_main)
catch_discover_tests(
        object_tests
        TEST_PREFIX
        "unittests."
        EXTRA_ARGS
        -s
        --reporter=xml
        --out=tests.xml)

add_executable(engine_tests engine/engine_tests.cpp)
target_link_libraries(engine_tests PRIVATE Engine catch_main)
catch_discover_tests(
        engine_tests
        TEST_PREFIX
        "unittests."
        EXTRA_ARGS
        -s
        --reporter=xml
        --out=tests.xml)