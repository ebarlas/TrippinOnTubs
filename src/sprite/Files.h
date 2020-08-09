#ifndef TRIPPINONTUBS_FILES_H
#define TRIPPINONTUBS_FILES_H

#include <sstream>

namespace trippin {
    constexpr int fileBufferSize = 128;
    std::stringstream readFile(const char *fileName);
}

#endif
