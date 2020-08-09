#ifndef TRIPPIN_SPRITEMETADATA_H
#define TRIPPIN_SPRITEMETADATA_H

#include "engine/Rect.h"

namespace trippin {
    class SpriteMetadata {
    public:
        void load(const std::string &name);
        int getFrames() const;
        int getDuration() const;
        Rect<int> getHitBox() const;
    private:
        int frames{};
        int duration{};
        Rect<int> hitBox{};
        static std::string getMetadataFile(const std::string& name);
    };
}

#endif
