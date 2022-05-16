#include "ScrollingScoreBoard.h"
#include "engine/Convert.h"

trippin::ScrollingScoreBoard::ScrollingScoreBoard(
        const Point<int> &windowSize,
        double scrollRate,
        SpriteManager &spriteManager,
        const RenderClock &renderClock) :
        renderClock(renderClock),
        windowSize(windowSize),
        scrollRate(scrollRate),
        scoreBoard(spriteManager.get("alpha")) {

}

void trippin::ScrollingScoreBoard::setScores(std::vector<Score> scores) {
    scoreBoard.setScores(std::move(scores));
}

void trippin::ScrollingScoreBoard::reset() {
    startTime = renderClock.getTicks();
}

void trippin::ScrollingScoreBoard::render() {
    auto delta = renderClock.getTicks() - startTime;
    scrollTop = toInt(static_cast<double>(delta.count()) * scrollRate);

    int scrollWrap = scrollTop % (windowSize.y + scoreBoard.getHeight());
    Point<int> position;
    position.x = (windowSize.x - scoreBoard.getWidth()) / 2;
    position.y = scrollWrap + windowSize.y;
    scoreBoard.render(position);
}