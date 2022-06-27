#ifndef TRIPPIN_SPRITEOBJECT_H
#define TRIPPIN_SPRITEOBJECT_H

#include "engine/Object.h"
#include "sprite/Sprite.h"
#include "Configuration.h"
#include "Map.h"

namespace trippin {
    class SpriteObject : public Object {
    public:
        SpriteObject(const Configuration::Object &configObject, const Map::Object &object, const Sprite &sprite);
        virtual ~SpriteObject() = default;
    protected:
        const Sprite &sprite;
    };
}

#endif
