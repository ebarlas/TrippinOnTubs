#ifndef TRIPPIN_SPRITEOBJECT_H
#define TRIPPIN_SPRITEOBJECT_H

#include "engine/Object.h"
#include "sprite/Sprite.h"
#include "sprite/Camera.h"
#include "Renderable.h"
#include "Configuration.h"
#include "Map.h"
#include "Mutex.h"

namespace trippin {
    class SpriteObject : public Object, public Renderable {
    public:
        virtual void init(const Configuration &config, const Map::Object &obj, const Sprite &spr);
        void render(const Camera &camera) override;
        void afterTick(Uint32 engineTicks) override;
    protected:
        const Sprite *sprite{};
        Mutex mutex;
        virtual Point<int> getPosition();
        virtual int getFrame();
        virtual bool isVisible();
    private:
        struct Channel {
            Point<int> roundedPosition;
        };
        Channel channel;
    };
}

#endif
