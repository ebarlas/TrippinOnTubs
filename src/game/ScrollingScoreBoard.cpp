#include "ScrollingScoreBoard.h"
#include "engine/Convert.h"

void trippin::ScrollingScoreBoard::setScores(std::vector<Score> scores) {
    scoreBoard.setScores(std::move(scores));
}

void trippin::ScrollingScoreBoard::setScrollRate(double rate) {
    scrollRate = rate;
}

void trippin::ScrollingScoreBoard::init(const trippin::Point<int> &ws, trippin::SpriteManager &spriteManager) {
    windowSize = ws;
    scoreBoard.init(ws, spriteManager);
}

void trippin::ScrollingScoreBoard::reset() {
    startTime = SDL_GetTicks();
}

void trippin::ScrollingScoreBoard::render() {
    scroll();
    scoreBoard.render();
}

void trippin::ScrollingScoreBoard::scroll() {
    int delta = static_cast<int>(SDL_GetTicks() - startTime);
    applyScroll(toInt(delta * scrollRate));
}

void trippin::ScrollingScoreBoard::applyScroll(int scrollTop) {
    int scrollWrap = scrollTop % (windowSize.y + scoreBoard.getHeight());
    scoreBoard.setTop(scrollWrap + windowSize.y);
}