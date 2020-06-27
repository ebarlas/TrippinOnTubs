#include <sstream>
#include <iomanip>
#include "RenderableObject.h"

std::string trippin::RenderableObject::format(double d, int precision) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << (d >= 0 ? "" : "_") << std::abs(d);
    return stream.str();
}

void trippin::RenderableObject::render(SDL_Renderer *renderer) {
    SDL_Rect rect{roundedPosition.x, roundedPosition.y, size.x, size.y};
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
    auto posLabel = format(position.x) + ", " + format(position.y);
    auto velLabel = format(velocity.x) + ", " + format(velocity.y);
    if (displayLabel) {
        fontRenderer->render(std::to_string(id), fontColor, roundedPosition + trippin::Vector<int>{5, 10});
        fontRenderer->render(posLabel, fontColor, roundedPosition + trippin::Vector<int>{5, 30});
        fontRenderer->render(velLabel, fontColor, roundedPosition + trippin::Vector<int>{5, 50});
    }
}