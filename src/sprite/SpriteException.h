#ifndef TRIPPIN_SPRITEEXCEPTION_H
#define TRIPPIN_SPRITEEXCEPTION_H

#include <string>

namespace trippin {
    class SpriteException : public std::exception {
    private:
        std::string message;
    public:
        SpriteException(std::string &&message);
        const std::string& getMessage() const;
        static void throwSdlError(std::string &&prefix);
        static void throwSdlImageError(std::string &&prefix);
    };
}

#endif
