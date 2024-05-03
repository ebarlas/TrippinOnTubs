#include <sstream>
#include "MyScores.h"
#include "sprite/Files.h"
#include "SDL.h"

trippin::MyScores::MyScores(int version, unsigned long limit) :
        version(version),
        limit(limit),
        latestScores{fileName("latest_", version), {}},
        topScores{fileName("top_", version), {}} {
    loadScores(latestScores);
    loadScores(topScores);
}

std::string trippin::MyScores::fileName(std::string_view type, int version) {
    std::stringstream fileName;
    fileName << "scores_" << type << version << ".json";
    return fileName.str();
}

void trippin::MyScores::loadScores(Scores &sc) {
    auto& [filename, scores] = sc;
    auto json = readPrefFile(filename.c_str());
    if (json) {
        auto j = nlohmann::json::parse(*json);
        try {
            j.get_to(scores);
        } catch (nlohmann::json::exception &e) {
            SDL_Log("scores file json parse error, message=%s", e.what());
        }
    }
}

static bool scoreOrder(const trippin::Score &left, const trippin::Score &right) {
    return left.score > right.score;
}

void trippin::MyScores::addScore(const trippin::Score &score) {
    addLatestScore(score);
    addTopScore(score);
}

void trippin::MyScores::addLatestScore(const trippin::Score &score) {
    auto& [filename, scores] = latestScores;
    auto at = scores.begin();
    scores.insert(at, score);
    resizeAndStore(filename, scores);
}

void trippin::MyScores::addTopScore(const trippin::Score &score) {
    auto& [filename, scores] = topScores;
    auto at = std::upper_bound(scores.begin(), scores.end(), score, scoreOrder);
    scores.insert(at, score);
    resizeAndStore(filename, scores);
}

void trippin::MyScores::resizeAndStore(std::string &filename, std::vector<Score> &scores) const {
    if (scores.size() > limit) {
        scores.resize(limit);
    }
    writePrefFile(filename.c_str(), nlohmann::json(scores).dump());
}

std::vector<trippin::Score> trippin::MyScores::getLatestScores() const {
    return latestScores.scores;
}

std::vector<trippin::Score> trippin::MyScores::getTopScores() const {
    return topScores.scores;
}
