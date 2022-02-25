#include "TrainingProgram.h"

std::array<const char *, 7> trippin::TrainingProgram::names{
        "jump",
        "duck",
        "stop",
        "charged_jump",
        "charged_duck_jump",
        "double_jump",
        "jump_slam_down"};

trippin::TrainingProgram::TrainingProgram(
        Point<int> windowSize,
        int margin,
        const Configuration &configuration,
        SpriteManager &spriteManager,
        SoundManager &soundManager,
        const Goggin &goggin,
        const RenderClock &renderClock,
        SceneBuilder &sceneBuilder,
        int zIndex) :
        goggin(goggin),
        margin(margin),
        stage(0),
        stageTicks(0),
        sceneBuilder(sceneBuilder),
        zIndex(zIndex),
        sound(soundManager.getEffect("chime2")),
        finishedWaitTicks(2 * (int) configuration.ticksPerSecond()),
        titleSprites(makeSprites(spriteManager)),
        controlSprites(makeSprites(spriteManager, "_controls")),
        windowSize(windowSize),
        titleInterpolator(renderClock, 750, 0),
        controlInterpolator(renderClock, 750, 0),
        complete(false) {
}

bool trippin::TrainingProgram::completed() {
    return complete;
}

void trippin::TrainingProgram::afterTick(Uint32 engineTicks) {
    if (stageTicks == 0) {
        stageTicks = engineTicks;
        resetInterpolators();
    }
    if (stage < names.size() &&
        lastEventTimes[stage](goggin) > stageTicks) { // event time for this stage exceeds time for prior stage
        stage = stage + 1;
        stageTicks = engineTicks;
        Mix_PlayChannel(-1, sound, 0);
        if (stage < names.size()) { // next stage exists
            resetInterpolators();
        }
    }
    if (stage == names.size() && engineTicks > stageTicks + finishedWaitTicks) { // final stage completed
        complete = true;
    }

    if (stage < names.size()) {  // next stage exists
        auto titleSprite = titleSprites[stage];
        auto controlSprite = controlSprites[stage];
        Point<int> menuPoint{titleInterpolator.interpolate(), windowSize.y / 5};
        Point<int> controlPoint{controlInterpolator.interpolate(), windowSize.y - controlSprite->getSize().y - margin};
        sceneBuilder.dispatch([menuPoint, controlPoint, titleSprite, controlSprite]() {
            titleSprite->render(menuPoint, 0);
            controlSprite->render(controlPoint, 0);
        }, zIndex);
    }
}

void trippin::TrainingProgram::resetInterpolators() {
    auto titleSpriteSize = titleSprites[stage]->getSize();
    auto controlSpriteSize = controlSprites[stage]->getSize();
    titleInterpolator.setMagnitude(titleSpriteSize.x + (windowSize.x - titleSpriteSize.x) / 2);
    titleInterpolator.setOffset(-titleSpriteSize.x);
    titleInterpolator.reset();
    controlInterpolator.setMagnitude(margin + controlSpriteSize.x);
    controlInterpolator.setOffset(-controlSpriteSize.x);
    controlInterpolator.reset();
}

std::array<const trippin::Sprite *, trippin::TrainingProgram::NUM_STAGES> trippin::TrainingProgram::makeSprites(
        SpriteManager &spriteManager, const std::string &suffix) {
    std::array<const Sprite *, NUM_STAGES> sprites{};
    auto it = names.begin();
    for (auto &elem: sprites) {
        elem = &spriteManager.get(*it + suffix);
        it++;
    }
    return sprites;
}
