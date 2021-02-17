#include "SDL.h"
#include "catch2/catch.hpp"
#include "game/SoundManager.h"

TEST_CASE("Sound manager test", "[sound]")
{
    SDL_Init(SDL_INIT_AUDIO);
    Mix_Init(MIX_INIT_MP3);
    Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048);
    trippin::SoundManager sm;
    REQUIRE(sm.getMusic("track1"));
    REQUIRE(sm.getEffect("chime1"));
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
}