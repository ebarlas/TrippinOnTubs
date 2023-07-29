#include "game/Game.h"

int main(int argc, char *argv[]) {
    trippin::Game game{"config"};
    game.init();
    game.start();
    return 0;
}