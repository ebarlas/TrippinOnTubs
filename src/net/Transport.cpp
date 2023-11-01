#include <sstream>
#include "Transport.h"
#include "Tcp.h"
#include "SDL_net.h"

trippin::Transport::Transport(std::string host, int port, int version, int limit)
        : host(std::move(host)), port(port), version(version), limit(limit) {

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
    msg += "?version=";
    msg += std::to_string(version);
    msg += "&limit=";
    msg += std::to_string(limit);
    msg += " HTTP/1.0\r\n";
    msg += "Host: ";
    msg += host;
    msg += "\r\n";
    msg += "Accept: */*\r\n\r\n";

    tcp.send(msg);
    std::string response = tcp.receive(1'024 * 64);

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

    nlohmann::json j = score.to_json();
    j["version"] = version;

    std::stringstream ss;
    ss << score.id << score.game;
    auto hashstr = ss.str();
    auto hashfn = [](const int &sum, const char &ch) { return sum + ch; };
    auto hash = std::accumulate(hashstr.begin(), hashstr.end(), 0, hashfn);

    auto json = j.dump();

    std::string msg = "POST /scores?h=";
    msg += std::to_string(hash);
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
    std::string response = tcp.receive(4'096);
    return response.find(" 200 OK") != std::string::npos;
}

bool trippin::Transport::addLogEvent(const LogEvent &event) const {
    Tcp tcp(host, port);
    auto sock = tcp.get();
    if (sock == nullptr) {
        return false;
    }

    nlohmann::json j;
    j["id"] = event.id;
    j["index"] = event.index;
    j["message"] = event.message;

    std::stringstream ss;
    ss << event.id << event.index;
    auto hashstr = ss.str();
    auto hashfn = [](const int &sum, const char &ch) { return sum + ch; };
    auto hash = std::accumulate(hashstr.begin(), hashstr.end(), 0, hashfn);

    auto json = j.dump();

    std::string msg = "POST /logs?h=";
    msg += std::to_string(hash);
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
    std::string response = tcp.receive(4'096);
    return response.find(" 200 OK") != std::string::npos;
}