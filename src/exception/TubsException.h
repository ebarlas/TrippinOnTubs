#ifndef TRIPPIN_TUBSEXCEPTION_H
#define TRIPPIN_TUBSEXCEPTION_H

#include <string>

namespace trippin {
    class TubsException : public std::exception {
    private:
        std::string message;
    public:
        TubsException(std::string &&message);
        const std::string& getMessage() const;
    };
}

#endif
