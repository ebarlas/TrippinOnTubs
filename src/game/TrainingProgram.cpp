#include "TrainingProgram.h"

trippin::TrainingProgram::TrainingProgram(
        Point<int> windowSize,
        const Configuration &configuration,
        SpriteManager &spriteManager,
        SoundManager &soundManager,
        const Goggin &goggin,
        const RenderClock &renderClock,
        SceneBuilder &sceneBuilder,
        int zIndex) :
        goggin(goggin),
        stage(TrainingStage::jump),
        stageTicks(0),
        menuLayout({windowSize.x, windowSize.y / 2}, 750, renderClock),
        sceneBuilder(sceneBuilder),
        zIndex(zIndex),
        sound(soundManager.getEffect("chime2")),
        finishedWaitTicks(2 * (int) configuration.ticksPerSecond()),
        sprites(makeSprites(spriteManager)),
        channel(0) {
}

bool trippin::TrainingProgram::completed() {
    return channel == TrainingStage::finished;
}

void trippin::TrainingProgram::afterTick(Uint32 engineTicks) {
    if (getLastEventTime() > stageTicks) {
        stage = stage + 1;
        stageTicks = engineTicks;
        channel = stage;
        Mix_PlayChannel(-1, sound, 0);
    }
    if (stage == TrainingStage::finishedWait && engineTicks > stageTicks + finishedWaitTicks) {
        channel = TrainingStage::finished;
    }

    auto ch = &channel;
    auto mu = &menuLayout;
    auto sp = sprites;
    sceneBuilder.dispatch([ch, mu, sp]() {
        auto st = ch->exchange(-1);
        if (st >= 0 && st < NUM_STAGES) {
            mu->setSprite(0, sp[st]);
            mu->init();
            mu->reset();
        }
        mu->render();
    }, zIndex);
}

Uint32 trippin::TrainingProgram::getLastEventTime() const {
    if (stage == TrainingStage::jump) {
        return goggin.getLastJumpTicks();
    } else if (stage == TrainingStage::duck) {
        return goggin.getLastDuckTicks();
    } else if (stage == TrainingStage::chargedJump) {
        return goggin.getLastChargedJumpTicks();
    } else if (stage == TrainingStage::duckJump) {
        return goggin.getLastDuckJumpTicks();
    } else if (stage == TrainingStage::doubleJump) {
        return goggin.getLastDoubleJumpTicks();
    }
    return 0;
}

std::array<const trippin::Sprite *, trippin::TrainingProgram::NUM_STAGES> trippin::TrainingProgram::makeSprites(
        SpriteManager &spriteManager) {
    std::array<const trippin::Sprite *, NUM_STAGES> sprites{};
    sprites[TrainingStage::jump] = &spriteManager.get("jump");
    sprites[TrainingStage::duck] = &spriteManager.get("duck");
    sprites[TrainingStage::chargedJump] = &spriteManager.get("charged_jump");
    sprites[TrainingStage::duckJump] = &spriteManager.get("charged_duck_jump");
    sprites[TrainingStage::doubleJump] = &spriteManager.get("double_jump");
    return sprites;
}