#include "TitleOverlay.h"
#include "engine/Convert.h"

trippin::TitleOverlay::TitleOverlay(
        const Point<int> &windowSize,
        Options options,
        SpriteManager &spriteManager,
        const RenderClock &renderClock) :
        renderClock(renderClock),
        windowSize(windowSize),
        options(options),
        titleSprite(spriteManager.get("trippin")),
        todaySprite(spriteManager.get("today")),
        allTimeSprite(spriteManager.get("all_time")),
        todayScoreBoard(spriteManager.get("alpha")),
        allTimeScoreBoard(spriteManager.get("alpha")),
        scoresSet(false) {

}

void trippin::TitleOverlay::render() {
    int scrollTop = getScrollTop();

    int height = windowSize.y * 3
                 + todaySprite.getDeviceSize().y
                 + todayScoreBoard.getHeight()
                 + allTimeSprite.getDeviceSize().y
                 + allTimeScoreBoard.getHeight();
    int scrollWrap = scrollTop % height;

    Point<int> titlePosition;
    titlePosition.x = (windowSize.x - titleSprite.getDeviceSize().x) / 2;
    titlePosition.y = scrollTop + (windowSize.y - titleSprite.getDeviceSize().y) / 2;
    titleSprite.renderDevice(titlePosition, 0);

    Point<int> todayPosition;
    todayPosition.x = (windowSize.x - todaySprite.getDeviceSize().x) / 2;
    todayPosition.y = scrollWrap + windowSize.y * 2;
    todaySprite.renderDevice(todayPosition, 0);
    int todayHeight = todaySprite.getDeviceSize().y + todayScoreBoard.getHeight();

    Point<int> todayScorePosition;
    todayScorePosition.x = (windowSize.x - todayScoreBoard.getWidth()) / 2;;
    todayScorePosition.y = scrollWrap + windowSize.y * 2 + todaySprite.getDeviceSize().y;
    todayScoreBoard.render(todayScorePosition);

    Point<int> allTimePosition;
    allTimePosition.x = (windowSize.x - allTimeSprite.getDeviceSize().x) / 2;
    allTimePosition.y = scrollWrap + windowSize.y * 3 + todayHeight;
    allTimeSprite.renderDevice(allTimePosition, 0);

    Point<int> allTimeScorePosition;
    allTimeScorePosition.x = (windowSize.x - allTimeScoreBoard.getWidth()) / 2;
    allTimeScorePosition.y = scrollWrap + windowSize.y * 3 + todayHeight + allTimeSprite.getDeviceSize().y;
    allTimeScoreBoard.render(allTimeScorePosition);
}

void trippin::TitleOverlay::setScores(std::vector<Score> today, std::vector<Score> top) {
    todayScoreBoard.setScores(std::move(today));
    allTimeScoreBoard.setScores(std::move(top));
    scoresSet = true;
    scoresSetTicks = renderClock.getTicks();
}

int trippin::TitleOverlay::getScrollTop() const {
    if (scoresSet) {
        auto duration = renderClock.getTicks() - scoresSetTicks;
        auto delta = static_cast<int>(duration.count()) - options.titlePause;
        if (delta > 0) {
            return toInt(delta * options.scrollRate);
        }
    }
    return 0;
}

bool trippin::TitleOverlay::hasScores() const {
    return scoresSet;
}
