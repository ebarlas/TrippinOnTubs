#include "game/Game.h"

int main(int argc, char *argv[]) {
    trippin::Game game{"test"};
    game.init();
    game.start();
    return 0;
}