#include <sstream>
#include "MyScores.h"
#include "sprite/Files.h"
#include "SDL.h"

trippin::MyScores::MyScores(trippin::MyScores::Type type, int version, int limit)
        : type(type), version(version), limit(limit) {
    auto fn = fileName();
    auto json = readPrefFile(fn.c_str());
    if (json) {
        auto j = nlohmann::json::parse(*json);
        try {
            j.get_to(scores);
        } catch (nlohmann::json::exception &e) {
            SDL_Log("scores file json parse error, message=%s", e.what());
        }
    }
}

std::string trippin::MyScores::fileName() const {
    std::stringstream fileName;
    fileName << "scores_" << (type == Type::top ? "top_" : "latest_") << version << ".json";
    return fileName.str();
}

static bool scoreOrder(const trippin::Score &left, const trippin::Score &right) {
    return left.score > right.score;
}

void trippin::MyScores::addScore(const trippin::Score &score) {
    auto at = type == Type::latest
            ? scores.begin()
            : std::upper_bound(scores.begin(), scores.end(), score, scoreOrder);
    scores.insert(at, score);
    if (scores.size() > limit) {
        scores.resize(limit);
    }
    auto fn = fileName();
    writePrefFile(fn.c_str(), nlohmann::json(scores).dump());
}

std::vector<trippin::Score> trippin::MyScores::getScores() const {
    return scores;
}
