#ifndef TRIPPIN_CONFIGURATION_H
#define TRIPPIN_CONFIGURATION_H

#include <string>
#include <sstream>

namespace trippin {
    class Configuration {
    public:
        void load(const std::string &name);
        int getMinPlayerSpan() const;
        int getPlayerBaseWidth() const;
    private:
        int minPlayerSpan;
        int playerBaseWidth;
        static std::string getConfigFile(const std::string& name);
    };
}

#endif
