#ifndef TRIPPINONTUBS_FILES_H
#define TRIPPINONTUBS_FILES_H

#include <string>
#include <optional>

namespace trippin {
    std::string readFile(const char *fileName);
    std::optional<std::string> readPrefFile(const char *fileName);
    bool writePrefFile(const char *fileName, const std::string &ss);
}

#endif
