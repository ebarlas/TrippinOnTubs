#include <sstream>
#include <iomanip>
#include "SDL.h"
#include "SDL2/SDL_ttf.h"
#include "engine/object.h"

class FontRenderer {
public:
    SDL_Renderer *renderer;
    TTF_Font *font;

    FontRenderer(TTF_Font *font, SDL_Renderer *renderer) : font(font), renderer(renderer) {
    }

    void render(const std::string &text, const SDL_Color &color, const trippin::Vector<int> &pos) {
        auto surface = TTF_RenderText_Solid(font, text.c_str(), color);
        auto texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect clip{pos.x, pos.y, surface->w, surface->h};
        SDL_RenderCopyEx(renderer, texture, nullptr, &clip, 0, nullptr, SDL_FLIP_NONE);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
};

std::string format(double d, int precision = 2) {
    std::stringstream stream;
    stream << std::fixed << std::setprecision(precision) << (d >= 0 ? "" : "_") << std::abs(d);
    return stream.str();
}

class RenderableObject : public trippin::Object {
public:
    FontRenderer* fontRenderer{};
    SDL_Color color{};
    SDL_Color fontColor{};
    bool displayLabel{};

    void render(SDL_Renderer *renderer) {
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
};

void gameLoop(SDL_Renderer *ren, const trippin::Vector<int> &windowSize) {
    auto font = TTF_OpenFont("fonts/lazy.ttf", 16);
    FontRenderer fontRenderer{font, ren};

    int nextId = 1;
    int wallWidth = 50;
    bool displayLabels = true;

    int ticksPerFrame = 20;
    int ticksSquaredPerFrame = ticksPerFrame * ticksPerFrame;

    trippin::Vector<double> terminalVelocity = {20.0 / ticksPerFrame, 20.0 / ticksPerFrame};
    trippin::Vector<double> acceleration = {0, 0.2 / ticksSquaredPerFrame};

    RenderableObject leftWall;
    leftWall.setId(nextId++);
    leftWall.setPlatform(true);
    leftWall.setSize({wallWidth, windowSize.y});
    leftWall.setPosition({0, 0});
    leftWall.color = {0, 0, 0, 255};
    leftWall.fontColor = {255, 255, 255, 255};
    leftWall.displayLabel = displayLabels;
    leftWall.fontRenderer = &fontRenderer;

    RenderableObject rightWall;
    rightWall.setId(nextId++);
    rightWall.setPlatform(true);
    rightWall.setSize({wallWidth, windowSize.y});
    rightWall.setPosition({static_cast<double>(windowSize.x - wallWidth), 0});
    rightWall.color = {0, 0, 0, 255};
    rightWall.fontColor = {255, 255, 255, 255};
    rightWall.displayLabel = displayLabels;
    rightWall.fontRenderer = &fontRenderer;

    RenderableObject floor;
    floor.setId(nextId++);
    floor.setPlatform(true);
    floor.setSize({windowSize.x, wallWidth});
    floor.setPosition({0, static_cast<double>(windowSize.y - wallWidth)});
    floor.color = {0, 0, 0, 255};
    floor.fontColor = {255, 255, 255, 255};
    floor.displayLabel = displayLabels;
    floor.fontRenderer = &fontRenderer;

    RenderableObject ceiling;
    ceiling.setId(nextId++);
    ceiling.setPlatform(true);
    ceiling.setSize({windowSize.x, wallWidth});
    ceiling.setPosition({0, 0});
    ceiling.color = {0, 0, 0, 255};
    ceiling.fontColor = {255, 255, 255, 255};
    ceiling.displayLabel = displayLabels;
    ceiling.fontRenderer = &fontRenderer;

    std::vector<trippin::Object *> objects{&leftWall, &rightWall, &floor, &ceiling};

    int numObjects = 10;
    for (int i = 0; i < numObjects; i++) {
        int width = 50 + std::rand() % 100;
        int height = 50 + std::rand() % 100;
        auto obj = new RenderableObject();
        obj->setId(nextId++);
        obj->setPlatform(false);
        obj->setSize({width, height});
        obj->setMass(width * height);
        obj->setPosition({static_cast<double>(std::rand() % (windowSize.x - width)),
                         static_cast<double>(std::rand() % (windowSize.y - height))});
        obj->setVelocity({
                static_cast<double>((std::rand() % static_cast<int>(terminalVelocity.x / 2 * 1000.0)) / 1000.0),
                static_cast<double>((std::rand() % static_cast<int>(terminalVelocity.y / 2 * 1000.0)) / 1000.0)});
        obj->setAcceleration(acceleration);
        obj->setTerminalVelocity(terminalVelocity * 2);
        obj->color = {static_cast<Uint8>(std::rand() % 128),
                      static_cast<Uint8>(std::rand() % 128),
                      static_cast<Uint8>(std::rand() % 128),
                      255};
        obj->fontColor = {255, 255, 255, 255};
        obj->displayLabel = displayLabels;
        obj->fontRenderer = &fontRenderer;
        objects.push_back(obj);
    }

    auto objPlatFn = [](trippin::Object &a, trippin::Object &b, const trippin::Sides &collision) {
        return a.reflectiveCollision(b, collision);
    };

    auto objObjFn = [](trippin::Object &a, trippin::Object &b, const trippin::Sides &collision) {
        return a.elasticCollision2D(b, collision);
    };

    bool debug = false;
    bool quit = false;
    while (!quit) {
        SDL_Event e;
        bool enterPressed = false;
        bool spacePressed = false;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                    enterPressed = true;
                }
                if (e.key.keysym.scancode == SDL_SCANCODE_SPACE) {
                    spacePressed = true;
                }
            }
        }

        if (enterPressed) {
            debug = !debug;
        }

        if (!debug || spacePressed) {
            for (int i = 0; i < ticksPerFrame; i++) {
                trippin::Object::applyMotion(objects);
                trippin::Object::snapObjects(objects);
                trippin::Object::applyPhysics(objects, objPlatFn, objObjFn);
            }
        }

        SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
        SDL_RenderClear(ren);

        for (auto obj : objects) {
            (reinterpret_cast<RenderableObject *>(obj))->render(ren);
        }

        SDL_RenderPresent(ren);
    }
}

int main() {
    trippin::Vector<int> windowSize{1600, 900};
    trippin::Vector<int> windowPos{100, 100};
    auto title = "Hacking";

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow(title, windowPos.x, windowPos.y, windowSize.x, windowSize.y, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    TTF_Init();

    gameLoop(ren, windowSize);

    TTF_Quit();
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}