#include <sstream>
#include "Transport.h"
#include "Tcp.h"
#include "SDL_net.h"

trippin::Transport::Transport(std::string host, int port) : host(std::move(host)), port(port) {

}

trippin::Transport::Scores trippin::Transport::topScores() const {
    return sendRequest("/scores/alltime");
}

trippin::Transport::Scores trippin::Transport::todayScores() const {
    return sendRequest("/scores/today");
}

trippin::Transport::Scores trippin::Transport::sendRequest(const std::string &uri) const {
    Tcp tcp(host, port);
    auto sock = tcp.get();
    if (sock == nullptr) {
        return {};
    }

    std::string msg = "GET ";
    msg += uri;
    msg += " HTTP/1.0\r\n";
    msg += "Host: ";
    msg += host;
    msg += "\r\n";
    msg += "Accept: */*\r\n\r\n";

    tcp.send(msg);
    std::string response = tcp.receive();

    auto f = response.find("\r\n\r\n");
    if (f == std::string::npos) {
        SDL_Log("response parse delimiter not found");
        return {};
    }

    Scores scores;
    auto json = response.substr(f + 4);
    try {
        nlohmann::json j = nlohmann::json::parse(json);
        j.get_to(scores.scores);
        scores.ok = true;
    } catch (nlohmann::json::exception &e) {
        SDL_Log("json parse error, message=%s", e.what());
        scores.ok = false;
    }

    return scores;
}

bool trippin::Transport::addScore(const Score &score) const {
    Tcp tcp(host, port);
    auto sock = tcp.get();
    if (sock == nullptr) {
        return false;
    }

    nlohmann::json j;
    j["id"] = score.id;
    j["name"] = score.name;
    j["score"] = score.score;

    std::stringstream hash;
    hash << std::hex << score.score << score.id;

    auto json = j.dump();

    std::string msg = "POST /scores?h=";
    msg += hash.str();
    msg += " HTTP/1.0\r\n";
    msg += "Host: ";
    msg += host;
    msg += "\r\n";
    msg += "Content-Type: application/json\r\n";
    msg += "Content-Length: ";
    msg += std::to_string(json.size());
    msg += "\r\n";
    msg += "Accept: */*\r\n\r\n";
    msg += json;

    tcp.send(msg);
    std::string response = tcp.receive();
    return response.find(" 200 OK") != std::string::npos;
}

void trippin::from_json(const nlohmann::json &j, trippin::Score &score) {
    j.at("id").get_to(score.id);
    j.at("name").get_to(score.name);
    j.at("score").get_to(score.score);
}
