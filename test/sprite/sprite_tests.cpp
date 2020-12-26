#include "catch2/catch.hpp"
#include "sprite/Sprite.h"

TEST_CASE("Sprite load", "[sprite]")
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *win = SDL_CreateWindow("Test", 0, 0, 400, 300, SDL_WINDOW_SHOWN);
    REQUIRE(win != nullptr);

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
    REQUIRE(ren != nullptr);

    trippin::Sprite sprite{ren, "goggin", trippin::Scale{"medium", 1.0}};
    REQUIRE(sprite.getSize().x == 480);
    REQUIRE(sprite.getSize().y == 480);
}