#include "LevelBonuses.h"
#include "ui/DigitLayout.h"

trippin::LevelBonuses::LevelBonuses(
        const Configuration &config,
        const Goggin &goggin,
        const SpiritClock &spiritClock,
        NotificationDrawer &notificationDrawer,
        ScoreTicker &scoreTicker,
        PointCloudManager &pointCloudManager,
        Camera &camera,
        SoundManager &soundManager,
        SpriteManager &spriteManager,
        bool lastLevel,
        int extraLives) :
        config(config),
        goggin(goggin),
        spiritClock(spiritClock),
        notificationDrawer(notificationDrawer),
        scoreTicker(scoreTicker),
        pointCloudManager(pointCloudManager),
        camera(camera),
        levelBonusSprite(spriteManager.get("level_completed_bonus")),
        timeBonusSprite(spriteManager.get("level_time_bonus")),
        livesBonusSprite(spriteManager.get("extra_lives_bonus")),
        gameBonusSprite(spriteManager.get("game_completed_bonus")),
        digitsSprite(spriteManager.get("digits")),
        sound(soundManager.getEffect("chime2")),
        lastLevel(lastLevel),
        extraLives(extraLives),
        triggered(false) {
}

void trippin::LevelBonuses::afterTick(int engineTicks) {
    if (triggered) {
        return;
    }

    if (!goggin.belowUniverse() && !goggin.rightOfUniverse()) {
        return;
    }

    triggered = true;

    if (goggin.belowUniverse()) {
        return;
    }

    Mix_PlayChannel(-1, sound, 0);

    auto vp = camera.getViewport();
    Point<int> center{vp.x + (3 * vp.w) / 4, vp.y + vp.h / 4};

    scoreTicker.add(config.levelBonus);
    pointCloudManager.addPointCloud(center, config.levelBonus, engineTicks);
    notificationDrawer.add([this](Point<int> p) {
        levelBonusSprite.renderDevice(p, 0);
    }, levelBonusSprite.getDeviceSize());

    if (lastLevel) {
        scoreTicker.add(config.gameBonus);
        pointCloudManager.addPointCloud(center, config.gameBonus, engineTicks);
        notificationDrawer.add([this](Point<int> p) {
            gameBonusSprite.renderDevice(p, 0);
        }, gameBonusSprite.getDeviceSize());
    }

    auto clockBars = spiritClock.getNumClockBars();
    if (clockBars > 0) {
        auto digitsWidth = DigitLayout::measureWidth(digitsSprite, clockBars);
        auto totalWidth = timeBonusSprite.getDeviceSize().x + digitsWidth;
        for (int i = 0; i < clockBars; i++) {
            scoreTicker.add(config.clockBarBonus);
            pointCloudManager.addPointCloud(center, config.clockBarBonus, engineTicks);
        }
        notificationDrawer.add([this, clockBars, totalWidth](Point<int> p) {
            timeBonusSprite.renderDevice(p, 0);
            DigitLayout::renderDigits(digitsSprite, {p.x + totalWidth, p.y}, clockBars);
        }, {totalWidth, timeBonusSprite.getDeviceSize().y});
    }

    if (lastLevel && extraLives > 0) {
        auto digitsWidth = DigitLayout::measureWidth(digitsSprite, extraLives);
        auto totalWidth = livesBonusSprite.getDeviceSize().x + digitsWidth;
        for (int i = 0; i < extraLives; i++) {
            scoreTicker.add(config.extraLivesBonus);
            pointCloudManager.addPointCloud(center, config.extraLivesBonus, engineTicks);
        }
        notificationDrawer.add([this, totalWidth](Point<int> p) {
            livesBonusSprite.renderDevice(p, 0);
            DigitLayout::renderDigits(digitsSprite, {p.x + totalWidth, p.y}, extraLives);
        }, {totalWidth, livesBonusSprite.getDeviceSize().y});
    }
}