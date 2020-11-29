#include <sstream>
#include <iomanip>
#include <cmath>
#include "RenderableObject.h"

std::string trippin::RenderableObject::format(double d, int precision) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << (d >= 0 ? "" : "_") << std::abs(d);
    return stream.str();
}

void trippin::RenderableObject::render(SDL_Renderer *renderer, const Rect<int> &viewport) {
    if (roundedBox.intersect(viewport)) {
        SDL_Rect rect{roundedPosition.x - viewport.x, roundedPosition.y - viewport.y, size.x, size.y};
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &rect);
        auto posLabel = format(position.x) + ", " + format(position.y);
        auto velLabel = format(velocity.x) + ", " + format(velocity.y);
        if (displayLabel) {
            fontRenderer->render(std::to_string(id), fontColor, roundedPosition - viewport.corner() + trippin::Point<int>{5, 10});
            fontRenderer->render(posLabel, fontColor, roundedPosition - viewport.corner() + trippin::Point<int>{5, 30});
            fontRenderer->render(velLabel, fontColor, roundedPosition - viewport.corner() + trippin::Point<int>{5, 50});
        }
    }
}

void trippin::RenderableObject::setId(int n) {
    id = n;
}

void trippin::RenderableObject::setPlatform(bool p) {
    platform = p;
}

void trippin::RenderableObject::setSize(const trippin::Point<int> &s) {
    size = s;
    updateRounded();
}

void trippin::RenderableObject::setPosition(const trippin::Point<double> &p) {
    position = p;
    updateRounded();
}

void trippin::RenderableObject::setVelocity(const trippin::Point<double> &v) {
    velocity = v;
}

void trippin::RenderableObject::setTerminalVelocity(const trippin::Point<double> &v) {
    terminalVelocity = v;
}

void trippin::RenderableObject::setAcceleration(const trippin::Point<double> &a) {
    acceleration = a;
}

void trippin::RenderableObject::setMass(double m) {
    mass = m;
}

trippin::Point<double> trippin::RenderableObject::getPosition() const {
    return position;
}

trippin::Point<double> trippin::RenderableObject::getVelocity() const {
    return velocity;
}

trippin::Point<double> trippin::RenderableObject::getTerminalVelocity() const {
    return terminalVelocity;
}

double trippin::RenderableObject::getGravity() const {
    return gravity;
}

trippin::Point<double> trippin::RenderableObject::getFriction() const {
    return friction;
}

trippin::Rect<int> trippin::RenderableObject::getRoundedBox() const {
    return roundedBox;
}

trippin::Point<int> trippin::RenderableObject::getSize() const {
    return size;
}

void trippin::RenderableObject::setFriction(const Point<double> &f) {
    friction = f;
}

void trippin::RenderableObject::setGravity(double g) {
    gravity = g;
}

trippin::Sides trippin::RenderableObject::getPlatformCollisions() const {
    return platformCollisions;
}

trippin::Point<int> trippin::RenderableObject::getRoundedPosition() const {
    return roundedPosition;
}

trippin::Point<double> trippin::RenderableObject::getCenter() const {
    return center;
}

trippin::Point<int> trippin::RenderableObject::getRoundedCenter() const {
    return roundedCenter;
}

trippin::Point<double> trippin::RenderableObject::getAcceleration() const {
    return acceleration;
}

trippin::Sides trippin::RenderableObject::getObjectCollisions() const {
    return objectCollisions;
}

int trippin::RenderableObject::getId() const {
    return id;
}

void trippin::RenderableObject::setPlatformCollision(trippin::Collision *collision) {
    platformCollision.set(collision);
}