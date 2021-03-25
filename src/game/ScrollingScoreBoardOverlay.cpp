#include "ScrollingScoreBoardOverlay.h"
#include "engine/Convert.h"

void trippin::ScrollingScoreBoardOverlay::setScores(std::vector<Score> scores) {
    scoreBoard.setScores(std::move(scores));
}

void trippin::ScrollingScoreBoardOverlay::setScrollRate(double rate) {
    scrollRate = rate;
}

void trippin::ScrollingScoreBoardOverlay::init(const trippin::Point<int> &ws, trippin::SpriteManager &spriteManager) {
    windowSize = ws;
    scoreBoard.init(ws, spriteManager);
}

void trippin::ScrollingScoreBoardOverlay::reset() {
    startTime = SDL_GetTicks();
}

void trippin::ScrollingScoreBoardOverlay::render() {
    scroll();
    scoreBoard.render();
}

void trippin::ScrollingScoreBoardOverlay::scroll() {
    int delta = static_cast<int>(SDL_GetTicks() - startTime);
    applyScroll(toInt(delta * scrollRate));
}

void trippin::ScrollingScoreBoardOverlay::applyScroll(int scrollTop) {
    int scrollWrap = scrollTop % (windowSize.y + scoreBoard.getHeight());
    scoreBoard.setTop(scrollWrap + windowSize.y);
}