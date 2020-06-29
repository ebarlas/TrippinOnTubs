#ifndef TRIPPIN_RENDERABLEOBJECT_H
#define TRIPPIN_RENDERABLEOBJECT_H

#include <string>
#include "graphics/FontRenderer.h"
#include "engine/Object.h"

namespace trippin {
    class RenderableObject : public trippin::Object {
    public:
        trippin::FontRenderer *fontRenderer{};
        SDL_Color color{};
        SDL_Color fontColor{};
        bool displayLabel{};
        static std::string format(double d, int precision = 2);
        void render(SDL_Renderer *renderer, const Rect<int> &viewport);
    };
}

#endif
