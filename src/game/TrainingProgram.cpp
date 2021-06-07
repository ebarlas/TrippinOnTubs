#include "TrainingProgram.h"

trippin::TrainingProgram::TrainingProgram(
        Point<int> windowSize,
        Configuration &configuration,
        SpriteManager &spriteManager,
        SoundManager &soundManager,
        Goggin &goggin) :
        goggin(goggin),
        stage(TrainingStage::jump),
        stageTicks(0),
        menuLayout({windowSize.x, windowSize.y / 2}, 750) {
    sprites[TrainingStage::jump] = &spriteManager.get("jump");
    sprites[TrainingStage::duck] = &spriteManager.get("duck");
    sprites[TrainingStage::chargedJump] = &spriteManager.get("charged_jump");
    sprites[TrainingStage::duckJump] = &spriteManager.get("charged_duck_jump");
    sprites[TrainingStage::doubleJump] = &spriteManager.get("double_jump");
    sound = soundManager.getEffect("chime2");
    finishedWaitTicks = 2 * (int) configuration.ticksPerSecond();
    stageChannel.set(0);
}

void trippin::TrainingProgram::render() {
    auto st = stageChannel.get();
    if (st >= 0 && st < NUM_STAGES) {
        menuLayout.setSprite(0, sprites[st]);
        menuLayout.init();
        menuLayout.reset();
        stageChannel.set(-1);
    }
    menuLayout.render();
}

bool trippin::TrainingProgram::completed() {
    return stageChannel.get() == TrainingStage::finished;
}

void trippin::TrainingProgram::afterTick(Uint32 engineTicks) {
    if (getLastEventTime() > stageTicks) {
        stage = stage + 1;
        stageTicks = engineTicks;
        stageChannel.set(stage);
        Mix_PlayChannel(-1, sound, 0);
    }
    if (stage == TrainingStage::finishedWait && engineTicks > stageTicks + finishedWaitTicks) {
        stageChannel.set(TrainingStage::finished);
    }
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
