#include "TrainingProgram.h"

trippin::TrainingProgram::TrainingProgram(
        Point<int> windowSize,
        int margin,
        const Configuration &configuration,
        SpriteManager &spriteManager,
        SoundManager &soundManager,
        const LevelStats &stats,
        const RenderClock &renderClock,
        SceneBuilder &sceneBuilder,
        unsigned int stage) :
        titleSprite(spriteManager.get(NAMES[stage])),
        controlSprite(spriteManager.get(NAMES[stage] + std::string("_controls"))),
        stats(stats),
        windowSize(windowSize),
        margin(margin),
        stage(stage),
        titleInterpolator(renderClock, 750, 0),
        controlInterpolator(renderClock, 750, 0),
        finishedWaitTicks(static_cast<const int>(2 * configuration.ticksPerSecond())),
        stageTicks(0),
        sound(soundManager.getEffect("chime2")),
        sceneBuilder(sceneBuilder),
        complete(false),
        firstTick(true),
        stageDone(false) {
}

bool trippin::TrainingProgram::completed() {
    return complete;
}

void trippin::TrainingProgram::afterTick(int engineTicks) {
    if (firstTick) {
        firstTick = false;
        resetInterpolators();
    }
    if (!stageDone && stats.exists(STAGES[stage])) {
        stageDone = true;
        stageTicks = engineTicks;
        Mix_PlayChannel(-1, sound, 0);
    }
    if (stageDone && engineTicks > stageTicks + finishedWaitTicks) {
        complete = true;
    }
    Point<int> menuPoint{titleInterpolator.interpolate(), windowSize.y / 5};
    auto top = windowSize.y - controlSprite.getDeviceSize().y - margin;
    Point<int> controlPoint{controlInterpolator.interpolate(), top};
    sceneBuilder.dispatch([this, menuPoint, controlPoint]() {
        titleSprite.renderDevice(menuPoint, 0);
        controlSprite.renderDevice(controlPoint, 0);
    });
}

void trippin::TrainingProgram::resetInterpolators() {
    auto titleSpriteSize = titleSprite.getDeviceSize();
    auto controlSpriteSize = controlSprite.getDeviceSize();
    titleInterpolator.setMagnitude(titleSpriteSize.x + (windowSize.x - titleSpriteSize.x) / 2);
    titleInterpolator.setOffset(-titleSpriteSize.x);
    titleInterpolator.reset();
    controlInterpolator.setMagnitude(margin + controlSpriteSize.x);
    controlInterpolator.setOffset(-controlSpriteSize.x);
    controlInterpolator.reset();
}

unsigned int trippin::TrainingProgram::getStage() const {
    return stage;
}
