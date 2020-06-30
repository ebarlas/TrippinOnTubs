#ifndef TRIPPINONTUBS_SPRITETYPE_H
#define TRIPPINONTUBS_SPRITETYPE_H

namespace trippin {
    enum class SpriteType {
        goggin
    };

    constexpr SpriteType spriteTypes[] = {SpriteType::goggin};

    constexpr const char* getSpriteName(SpriteType type) {
        switch (type) {
            case SpriteType::goggin:
                return "goggin";
        }
    }
}

#endif
