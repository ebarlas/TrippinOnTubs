#ifndef TRIPPIN_SPRITEOBJECT_H
#define TRIPPIN_SPRITEOBJECT_H

#include "engine/Object.h"
#include "sprite/Sprite.h"
#include "sprite/Camera.h"
#include "Configuration.h"
#include "Map.h"

namespace trippin {
    class SpriteObject : public Object {
    public:
        SpriteObject();
        virtual ~SpriteObject();
        virtual void init(const Configuration &config, const Map::Object &obj, const Sprite &spr);
        virtual void render(SDL_Renderer* renderer, const Camera &camera);
        void afterTick(Uint32 engineTicks) override;
    protected:
        const Sprite *sprite{};
        SDL_mutex *mutex{};
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
