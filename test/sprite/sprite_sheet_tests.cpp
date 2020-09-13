#include "catch2/catch.hpp"
#include "sprite/SpriteSheet.h"

TEST_CASE("Sprite sheet load", "[sprite]")
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *win = SDL_CreateWindow("Test", 0, 0, 400, 300, SDL_WINDOW_SHOWN);
    REQUIRE(win != nullptr);

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
    REQUIRE(ren != nullptr);

    trippin::SpriteSheet sheet{ren, "goggin", trippin::Scale::medium};
    REQUIRE(sheet.getSize().x == 3840);
    REQUIRE(sheet.getSize().y == 480);
}