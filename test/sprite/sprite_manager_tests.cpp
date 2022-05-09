#include "catch2/catch.hpp"
#include "sprite/SpriteManager.h"

TEST_CASE("Sprite manager load", "[sprite]")
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *win = SDL_CreateWindow("Test", 0, 0, 400, 300, SDL_WINDOW_SHOWN);
    REQUIRE(win != nullptr);

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, 0);
    REQUIRE(ren != nullptr);

    trippin::SpriteLoader spriteLoader{trippin::Scale{"hdplus", 1.0}};
    trippin::SpriteManager manager{ren, spriteLoader, 5};
    auto &sprite = manager.get("goggin");
    REQUIRE(sprite.getSize().x == 90);
    REQUIRE(sprite.getSize().y == 90);
}