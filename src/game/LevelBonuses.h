#ifndef TRIPPIN_LEVELBONUSES_H
#define TRIPPIN_LEVELBONUSES_H

#include "engine/Listener.h"
#include "sprite/SpriteManager.h"
#include "game/Goggin.h"
#include "game/SpiritClock.h"
#include "game/ScoreTicker.h"

namespace trippin {
    class LevelBonuses : public Listener {
    private:
        const Configuration &config;
        const Goggin &goggin;
        const SpiritClock &spiritClock;
        NotificationDrawer &notificationDrawer;
        ScoreTicker &scoreTicker;
        PointCloudManager &pointCloudManager;
        Camera &camera;
        const Sprite &levelBonusSprite;
        const Sprite &timeBonusSprite;
        const Sprite &livesBonusSprite;
        const Sprite &gameBonusSprite;
        const Sprite &digitsSprite;
        Mix_Chunk *const sound;
        const bool lastLevel;
        const int extraLives;
        bool triggered;
    public:
        LevelBonuses(
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
                int extraLives);
        void afterTick(int engineTicks) override;
    };
}

#endif
