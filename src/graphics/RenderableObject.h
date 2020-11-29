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

        void setId(int id);
        void setPlatform(bool p);
        void setSize(const Point<int> &s);
        void setPosition(const Point<double> &p);
        void setVelocity(const Point<double> &v);
        void setTerminalVelocity(const Point<double> &v);
        void setAcceleration(const Point<double> &a);
        void setMass(double m);
        void setFriction(const Point<double> &f);
        void setGravity(double g);
        void setPlatformCollision(Collision *collision);
        int getId() const;
        Point<double> getPosition() const;
        Point<double> getCenter() const;
        Point<double> getVelocity() const;
        Point<double> getAcceleration() const;
        double getGravity() const;
        Point<double> getFriction() const;
        Point<double> getTerminalVelocity() const;
        Point<int> getSize() const;
        Rect<int> getRoundedBox() const;
        Point<int> getRoundedPosition() const;
        Point<int> getRoundedCenter() const;
        Sides getPlatformCollisions() const;
        Sides getObjectCollisions() const;
    };
}

#endif
