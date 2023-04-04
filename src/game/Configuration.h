#ifndef TRIPPIN_CONFIGURATION_H
#define TRIPPIN_CONFIGURATION_H

#include <string>
#include "game/Json.h"
#include "sprite/Scale.h"

namespace trippin {
    struct Configuration {
        struct Version {
            int major;
            int minor;
        };

        struct Object {
            std::string type;
            bool platform;
            double runningAcceleration{};
            double risingAcceleration{};
            double gravity{};
            double fallGravity{};
            double mass{};
            double massFactor{};
            double minJumpVelocity;
            double maxJumpVelocity;
            double maxDuckJumpVelocity;
            int minJumpChargeTime;
            int maxJumpChargeTime;
            int jumpSoundTimeout;
            Point<double> velocity{};
            Point<double> terminalVelocity{};
            Point<double> friction{};
            int dustPeriod{};
            double duckFriction{};
            double coefficient{};
            bool sparkle{};
            bool accelerateWhenGrounded{};
            bool randFrame{};
            bool elasticObjectCollisions{};
            int hitPoints{};
        };

        struct LayerObject {
            std::string type;
            bool randFrame{};
            Point<double> velocity{};
        };

        struct Scale {
            std::string name;
            int multiplier;
            int minWidth;
        };

        struct Db {
            std::string host;
            int port;
        };

        Version version;
        Point<int> windowSize;
        int spiritSecondsBehind;
        int tickRate;
        int spiritClockTickPeriod;
        int activationProximity;
        int deactivationProximity;
        int shakeDuration;
        int shakeHertz;
        int shakeAmplitude;
        int meterMargin;
        int fastReplayFactor;
        int scrollPixelsPerSecond;
        int minComboHits;
        int pointsPerObject;
        int pointsPerHitPoint;
        int pointsPerGroup;
        int pointsPerMember;
        int pointsPerComboHit;
        int timeBonusPoints;
        int timeBonusSeconds;
        int tubBonusPoints;
        int sparkleTubBonusPoints;
        int collisionDurationMilliseconds;
        int collisionFlashMilliseconds;
        int collisionCooldownMilliseconds;
        int healthBarVerticalClearanceFactor;
        int maxConsecutiveJumps;
        double knockoutVelocityFactor;
        double dustJumpFactor;
        double dustLandFactor;
        double dustRunFactor;
        std::string jumpSound;
        std::string stompSound;
        Point<int> healthBarSize;
        std::vector<std::string> maps;
        std::string loadMap;
        std::string trainMap;
        std::string loadAutoPlay;
        std::vector<Scale> scales;
        std::vector<std::string> prefetchSprites;
        std::vector<Object> objects;
        std::vector<LayerObject> layerObjects;
        Db db;
        void load(const std::string &name);
        static std::string getConfigFile(const std::string &name);

        double msPerTick() const;
        double ticksPerSecond() const;
        double engineTicksPerSpiritClockTick() const;
        const Object *findObject(const std::string &name) const;
        const LayerObject *findLayerObject(const std::string &name) const;

        void rescale(const trippin::Scale &scale);
    };

    void from_json(const nlohmann::json &j, Configuration &config);
    void from_json(const nlohmann::json& j, Configuration::Object& obj);
    void from_json(const nlohmann::json& j, Configuration::LayerObject& obj);
}

#endif
