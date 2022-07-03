#include "ScrollingScoreBoard.h"

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
    scoreBoard.render(renderPosition());
}

const trippin::Score *trippin::ScrollingScoreBoard::onClick(const Point<int> &coords) const {
    auto pos = renderPosition();
    auto offset = coords - pos;
    if (offset.x > 0 && offset.y > 0 && offset.x < scoreBoard.getWidth() && offset.y < scoreBoard.getHeight()) {
        int n = static_cast<int>(scoreBoard.numScores() * static_cast<double>(offset.y) / scoreBoard.getHeight());
        return &scoreBoard.scoreAt(n);
    }
    return nullptr;
}

trippin::Point<int> trippin::ScrollingScoreBoard::renderPosition() const {
    auto delta = renderClock.getTicks() - startTime;
    int scrollTop = static_cast<int>(static_cast<double>(delta.count()) * scrollRate);
    int scrollWrap = scrollTop % (windowSize.y + scoreBoard.getHeight());
    return {(windowSize.x - scoreBoard.getWidth()) / 2, scrollWrap + windowSize.y};
}
