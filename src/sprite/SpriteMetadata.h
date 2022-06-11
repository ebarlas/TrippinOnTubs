#ifndef TRIPPIN_SPRITEMETADATA_H
#define TRIPPIN_SPRITEMETADATA_H

#include "engine/Rect.h"

namespace trippin {
    class SpriteMetadata {
    public:
        // Load metadata file and overwrite properties of this object
        void load(const std::string &name);

        // Get total number of frames for the sprite
        int getFrames() const;

        // Duration per frame in milliseconds
        int getDuration() const;

        // Hit box rectangle from metadata file
        // This is a relative area within the larger sprite rectangle
        // Pixels in the base so-called unit sprite scale
        Rect<int> getHitBox() const;
    private:
        int frames{};
        int duration{};
        Rect<int> hitBox{};
        static std::string getMetadataFile(const std::string& name);
    };
}

#endif
