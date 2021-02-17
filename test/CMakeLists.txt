# automatically enable catch2 to generate ctest targets
find_package(Catch2 REQUIRED)
include(Catch)

add_library(catch_main STATIC catch_main.cpp)
target_link_libraries(catch_main Catch2::Catch2)

add_executable(engine_tests
        engine/engine_tests.cpp
        engine/object_tests.cpp
        engine/sides_tests.cpp
        engine/rect_tests.cpp
        engine/point_tests.cpp)
target_link_libraries(engine_tests PRIVATE Engine catch_main)
catch_discover_tests(
        engine_tests
        TEST_PREFIX
        "unittests."
        EXTRA_ARGS
        -s
        --reporter=xml
        --out=tests.xml)

add_executable(sprite_tests
        sprite/sprite_metadata_tests.cpp
        sprite/sprite_sheet_tests.cpp
        sprite/sprite_tests.cpp
        sprite/sprite_manager_tests.cpp)
target_link_libraries(sprite_tests PRIVATE Sprite catch_main)
catch_discover_tests(
        sprite_tests
        TEST_PREFIX
        "unittests."
        EXTRA_ARGS
        -s
        --reporter=xml
        --out=tests.xml)

add_executable(game_tests
        game/map_tests.cpp
        game/sound_manager_tests.cpp
        game/configuration_tests.cpp)
target_link_libraries(game_tests PRIVATE Game catch_main)
catch_discover_tests(
        game_tests
        TEST_PREFIX
        "unittests."
        EXTRA_ARGS
        -s
        --reporter=xml
        --out=tests.xml)