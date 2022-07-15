#include "ScrollingScoreBoard.h"

trippin::ScrollingScoreBoard::ScrollingScoreBoard(
        const Point<int> &windowSize,
        double scrollRate,
        SpriteManager &spriteManager,
        const RenderClock &renderClock) :
        renderClock(renderClock),
        windowSize(windowSize),
        scrollRate(scrollRate),
        scoreBoard(spriteManager.get("alpha")),
        highScoreTitle(spriteManager.get("high_score_title")) {
}

void trippin::ScrollingScoreBoard::setScores(std::vector<Score> scores) {
    scoreBoard.setScores(std::move(scores));
}

void trippin::ScrollingScoreBoard::reset() {
    startTime = renderClock.getTicks();
}

void trippin::ScrollingScoreBoard::render() {
    highScoreTitle.renderDevice(titleRenderPosition(), 0);
    scoreBoard.render(boardRenderPosition());
}

const trippin::Score *trippin::ScrollingScoreBoard::onClick(const Point<int> &coords) const {
    auto pos = boardRenderPosition();
    auto offset = coords - pos;
    if (offset.x > 0 && offset.y > 0 && offset.x < scoreBoard.getWidth() && offset.y < scoreBoard.getHeight()) {
        int n = static_cast<int>(scoreBoard.numScores() * static_cast<double>(offset.y) / scoreBoard.getHeight());
        return &scoreBoard.scoreAt(n);
    }
    return nullptr;
}

trippin::Point<int> trippin::ScrollingScoreBoard::titleRenderPosition() const {
    auto delta = renderClock.getTicks() - startTime;
    int scrollTop = static_cast<int>(static_cast<double>(delta.count()) * scrollRate);
    int totalHeight = titleHeight() + scoreBoard.getHeight();
    int scrollWrap = scrollTop % (windowSize.y + totalHeight);
    return {(windowSize.x - highScoreTitle.getDeviceSize().x) / 2, scrollWrap + windowSize.y};
}

trippin::Point<int> trippin::ScrollingScoreBoard::boardRenderPosition() const {
    auto pos = titleRenderPosition();
    return {(windowSize.x - scoreBoard.getWidth()) / 2, pos.y + titleHeight()};
}

int trippin::ScrollingScoreBoard::titleHeight() const {
    auto height = highScoreTitle.getDeviceSize().y;
    return height + height / 2;
}
