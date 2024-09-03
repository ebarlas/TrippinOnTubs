#ifndef TRIPPIN_MARQUEE_H
#define TRIPPIN_MARQUEE_H

#include <string>
#include <map>
#include "sprite/SpriteManager.h"
#include "sprite/RenderClock.h"

namespace trippin {
    class Marquee {
    public:
        explicit Marquee(
                double scrollPxPerMs,
                const Point<int> &windowSize,
                const Sprite &characters,
                const RenderClock &renderClock,
                int margin);
        void render();
        void start(const std::string &message);
        [[nodiscard]] bool started() const;
    private:
        struct Char {
            int frame;
            int x;
            int y;
        };
        struct Width {
            char c;
            float w;
        };
        constexpr static const Width CHAR_WIDTHS[] = {
                {'0',  0.8f},
                {'1',  0.5f},
                {'2',  0.7f},
                {'3',  0.65f},
                {'4',  0.8f},
                {'5',  0.65f},
                {'6',  0.75f},
                {'7',  0.65f},
                {'8',  0.7f},
                {'9',  0.7f},
                {'A',  0.8f},
                {'B',  0.6f},
                {'C',  0.7f},
                {'D',  0.7f},
                {'E',  0.75f},
                {'F',  0.65f},
                {'G',  0.7f},
                {'H',  0.7f},
                {'I',  0.45f},
                {'J',  0.75f},
                {'K',  0.7f},
                {'L',  0.65f},
                {'M',  1.0f},
                {'N',  0.7f},
                {'O',  0.75f},
                {'P',  0.7f},
                {'Q',  0.8f},
                {'R',  0.75f},
                {'S',  0.75f},
                {'T',  0.75f},
                {'U',  0.7f},
                {'V',  0.65f},
                {'W',  1.0f},
                {'X',  0.75f},
                {'Y',  0.75f},
                {'Z',  0.6f},
                {'.',  0.4f},
                {',',  0.5f},
                {'\'', 0.4f},
                {' ',  0.65f},
                {'\n', 0.9f},
                {'!',  0.5f},
        };
        const std::unordered_map<char, float> charWidths;
        const double scrollPxPerMs;
        const Point<int> &windowSize;
        const Sprite &characters;
        const RenderClock &renderClock;
        const int margin;
        std::vector<Char> message;
        std::chrono::milliseconds startTicks{};
        [[nodiscard]] int calculateLineWidth(const std::string &s, std::string::const_iterator it) const;
        [[nodiscard]] std::vector<Char> compile(const std::string &msg) const;
        [[nodiscard]] bool endOfLine(char c, int lineWidth) const;
        static int getFrame(char c);
        static std::unordered_map<char, float> makeCharWidths();
    };
}

#endif
