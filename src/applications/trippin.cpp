#include "game/Game.h"

int main(int argc, char *args[]) {
    trippin::Game game{"test"};
    game.init();
    game.start();
}