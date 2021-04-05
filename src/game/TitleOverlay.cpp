#include "TitleOverlay.h"
#include "engine/Convert.h"

void trippin::TitleOverlay::init(const Point<int> &ws, SpriteManager &spriteManager) {
    todayScoreBoard.init(ws, spriteManager);
    allTimeScoreBoard.init(ws, spriteManager);

    titleSprite = &spriteManager.get("trippin");
    todaySprite = &spriteManager.get("today");
    allTimeSprite = &spriteManager.get("all_time");

    titlePosition.x = (ws.x - titleSprite->getSize().x) / 2;
    todayPosition.x = (ws.x - todaySprite->getSize().x) / 2;
    allTimePosition.x = (ws.x - allTimeSprite->getSize().x) / 2;

    windowSize = ws;
    applyScroll(0);
}

void trippin::TitleOverlay::render() {
    scroll();
    if (inView(titlePosition.y, titleSprite->getSize().y))
        titleSprite->render(titlePosition, 0);
    if (inView(todayPosition.y, todaySprite->getSize().y))
        todaySprite->render(todayPosition, 0);
    if (inView(todayScoreBoard.getTop(), todayScoreBoard.getHeight()))
        todayScoreBoard.render();
    if (inView(allTimePosition.y, allTimeSprite->getSize().y))
        allTimeSprite->render(allTimePosition, 0);
    if (inView(allTimeScoreBoard.getTop(), allTimeScoreBoard.getHeight()))
        allTimeScoreBoard.render();
}

void trippin::TitleOverlay::setScores(std::vector<Score> today, std::vector<Score> top) {
    todayScoreBoard.setScores(std::move(today));
    allTimeScoreBoard.setScores(std::move(top));
    scoresSet = true;
    scoresSetTicks = SDL_GetTicks();
}

void trippin::TitleOverlay::setScrollRate(double sr) {
    scrollRate = sr;
}

void trippin::TitleOverlay::scroll() {
    if (scoresSet) {
        int delta = static_cast<int>(SDL_GetTicks() - scoresSetTicks) - titlePause;
        if (delta > 0) {
            applyScroll(toInt(delta * scrollRate));
        }
    }
}

void trippin::TitleOverlay::applyScroll(int scrollTop) {
    int height = windowSize.y * 3
                 + todaySprite->getSize().y + allTimeSprite->getSize().y
                 + todayScoreBoard.getHeight() + allTimeScoreBoard.getHeight();
    int scrollWrap = scrollTop % height;
    int todayHeight = todaySprite->getSize().y + todayScoreBoard.getHeight();
    titlePosition.y = scrollTop + (windowSize.y - titleSprite->getSize().y) / 2;
    todayPosition.y = scrollWrap + windowSize.y * 2;
    todayScoreBoard.setTop(scrollWrap + windowSize.y * 2 + todaySprite->getSize().y);
    allTimePosition.y = scrollWrap + windowSize.y * 3 + todayHeight;
    allTimeScoreBoard.setTop(scrollWrap + windowSize.y * 3 + todayHeight + allTimeSprite->getSize().y);
}

void trippin::TitleOverlay::setTitlePause(int pause) {
    titlePause = pause;
}

bool trippin::TitleOverlay::inView(int top, int h) const {
    int bottom = top + h;
    return (top >= 0 && top <= windowSize.y)
           || (bottom >= 0 && bottom <= windowSize.y)
           || (top <= 0 && bottom >= windowSize.y);
}

bool trippin::TitleOverlay::hasScores() const {
    return scoresSet;
}
