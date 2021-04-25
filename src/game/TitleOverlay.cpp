#include "TitleOverlay.h"
#include "engine/Convert.h"

trippin::TitleOverlay::TitleOverlay(const Point<int> &windowSize, Options options, SpriteManager &spriteManager) :
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
                 + todaySprite.getSize().y
                 + todayScoreBoard.getHeight()
                 + allTimeSprite.getSize().y
                 + allTimeScoreBoard.getHeight();
    int scrollWrap = scrollTop % height;

    Point<int> titlePosition;
    titlePosition.x = (windowSize.x - titleSprite.getSize().x) / 2;
    titlePosition.y = scrollTop + (windowSize.y - titleSprite.getSize().y) / 2;
    titleSprite.render(titlePosition, 0);

    Point<int> todayPosition;
    todayPosition.x = (windowSize.x - todaySprite.getSize().x) / 2;
    todayPosition.y = scrollWrap + windowSize.y * 2;
    todaySprite.render(todayPosition, 0);
    int todayHeight = todaySprite.getSize().y + todayScoreBoard.getHeight();

    Point<int> todayScorePosition;
    todayScorePosition.x = (windowSize.x - todayScoreBoard.getWidth()) / 2;;
    todayScorePosition.y = scrollWrap + windowSize.y * 2 + todaySprite.getSize().y;
    todayScoreBoard.render(todayScorePosition);

    Point<int> allTimePosition;
    allTimePosition.x = (windowSize.x - allTimeSprite.getSize().x) / 2;
    allTimePosition.y = scrollWrap + windowSize.y * 3 + todayHeight;
    allTimeSprite.render(allTimePosition, 0);

    Point<int> allTimeScorePosition;
    allTimeScorePosition.x = (windowSize.x - allTimeScoreBoard.getWidth()) / 2;
    allTimeScorePosition.y = scrollWrap + windowSize.y * 3 + todayHeight + allTimeSprite.getSize().y;
    allTimeScoreBoard.render(allTimeScorePosition);
}

void trippin::TitleOverlay::setScores(std::vector<Score> today, std::vector<Score> top) {
    todayScoreBoard.setScores(std::move(today));
    allTimeScoreBoard.setScores(std::move(top));
    scoresSet = true;
    scoresSetTicks = SDL_GetTicks();
}

int trippin::TitleOverlay::getScrollTop() const {
    if (scoresSet) {
        int delta = static_cast<int>(SDL_GetTicks() - scoresSetTicks) - options.titlePause;
        if (delta > 0) {
            return toInt(delta * options.scrollRate);
        }
    }
    return 0;
}

bool trippin::TitleOverlay::hasScores() const {
    return scoresSet;
}
