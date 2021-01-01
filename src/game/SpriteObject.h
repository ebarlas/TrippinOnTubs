#ifndef TRIPPIN_SPRITEOBJECT_H
#define TRIPPIN_SPRITEOBJECT_H

#include "engine/Object.h"
#include "sprite/Sprite.h"
#include "sprite/Camera.h"
#include "lock/Mutex.h"
#include "Renderable.h"
#include "Configuration.h"
#include "Map.h"

namespace trippin {
    class SpriteObject : public Object, public Renderable {
    public:
        virtual void init(const Configuration &config, const Map::Object &obj, const Sprite &spr);
    protected:
        const Sprite *sprite{};
    };
}

#endif
