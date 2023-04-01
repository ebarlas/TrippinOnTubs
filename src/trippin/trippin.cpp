#include "game/Game.h"

int main() {
    trippin::Game game{"test"};
    game.init();
    game.start();
    return 0;
}