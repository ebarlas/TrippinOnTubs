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
        unsigned int stage,
        int progress) :
        titleSprite(spriteManager.get(NAMES[stage])),
        controlSprite(spriteManager.get(NAMES[stage] + std::string("_controls"))),
        statusSprite(spriteManager.get("training_status")),
        stats(stats),
        windowSize(windowSize),
        margin(margin),
        stage(stage),
        titleInterpolator(renderClock, 750, 0),
        controlInterpolator(renderClock, 750, 0),
        finishedWaitTicks(static_cast<const int>(2 * configuration.ticksPerSecond())),
        stageTicks(0),
        sound(soundManager.getEffect("chime3")),
        sceneBuilder(sceneBuilder),
        complete(false),
        firstTick(true),
        stageDone(false),
        startProgress(progress),
        progress(progress),
        trainingProgress(progress),
        lastTubTime(0),
        tubCount(0) {
}

bool trippin::TrainingProgram::completed() {
    return complete;
}

void trippin::TrainingProgram::afterTick(int engineTicks) {
    if (firstTick) {
        firstTick = false;
        slideIn();
    }
    if (!stageDone) {
        if (STAGES[stage] == LevelStats::Event::JumpSlamDown
            || STAGES[stage] == LevelStats::Event::DoubleJump
            || STAGES[stage] == LevelStats::Event::DuckJump) {
            auto jumpTime = stats.lastTime(LevelStats::Event::Jump);
            auto tubTime = stats.lastTime(LevelStats::Event::WingedTub);
            if (tubTime > lastTubTime) { // new tub time
                if (jumpTime > lastTubTime) { // intervening jump
                    tubCount = 0;
                }
                tubCount++;
                lastTubTime = tubTime;
                if (tubCount == TUBS_PER_JUMP) {
                    progress++;
                }
            }
        } else {
            progress = startProgress + stats.count(LevelStats::Event::WingedTub);
        }
        if (progress >= MOVES_PER_STAGE) {
            stageDone = true;
        }
        if (stageDone) {
            stageTicks = engineTicks;
            Mix_PlayChannel(-1, sound, 0);
            slideOut();
        }
    }
    if (stageDone && engineTicks > stageTicks + finishedWaitTicks) {
        complete = true;
    }
    Point<int> menuPoint{titleInterpolator.interpolate(), windowSize.y / 8};
    auto top = windowSize.y - controlSprite.getDeviceSize().y - margin;
    Point<int> controlPoint{controlInterpolator.interpolate(), top};
    Point<int> statusPoint{windowSize.x - statusSprite.getDeviceSize().x - margin, margin};
    auto statusFrame = std::min(MOVES_PER_STAGE, progress);
    trainingProgress = statusFrame;
    sceneBuilder.dispatch([this, menuPoint, controlPoint, statusPoint, statusFrame]() {
        titleSprite.renderDevice(menuPoint, 0);
        controlSprite.renderDevice(controlPoint, 0);
        statusSprite.renderDevice(statusPoint, statusFrame);
    });
}

void trippin::TrainingProgram::slideIn() {
    auto titleSpriteSize = titleSprite.getDeviceSize();
    auto controlSpriteSize = controlSprite.getDeviceSize();
    titleInterpolator.setMagnitude(titleSpriteSize.x + (windowSize.x - titleSpriteSize.x) / 2);
    titleInterpolator.setOffset(-titleSpriteSize.x);
    titleInterpolator.reset();
    controlInterpolator.setMagnitude(-(margin + controlSpriteSize.x));
    controlInterpolator.setOffset(windowSize.x);
    controlInterpolator.reset();
}

void trippin::TrainingProgram::slideOut() {
    auto titleSpriteSize = titleSprite.getDeviceSize();
    auto controlSpriteSize = controlSprite.getDeviceSize();
    titleInterpolator.setMagnitude(-(titleSpriteSize.x + (windowSize.x - titleSpriteSize.x) / 2));
    titleInterpolator.setOffset((windowSize.x - titleSpriteSize.x) / 2);
    titleInterpolator.reset();
    controlInterpolator.setMagnitude(margin + controlSpriteSize.x);
    controlInterpolator.setOffset(windowSize.x - (margin + controlSpriteSize.x));
    controlInterpolator.reset();
}

int trippin::TrainingProgram::getProgress() const {
    return trainingProgress;
}
