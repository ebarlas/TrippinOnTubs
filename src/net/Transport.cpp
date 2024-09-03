#include <sstream>
#include "Transport.h"
#include "Tcp.h"
#include "SDL_net.h"

trippin::Transport::Transport(std::string host, int port, int major, int minor, int limit)
        : host(std::move(host)), port(port), major(major), minor(minor), limit(limit) {
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

    std::stringstream req;
    req << "GET " << uri
        << "?version=" << major
        << "&limit=" << limit
        << "&compression=diff"
        << " HTTP/1.0\r\n"
        << "Host: " << host << "\r\n"
        << "Accept: */*\r\n\r\n";

    tcp.send(req.str());
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

trippin::AddResult trippin::Transport::addScore(const Score &score) const {
    Tcp tcp(host, port);
    auto sock = tcp.get();
    if (sock == nullptr) {
        return AddResult::other;
    }

    nlohmann::json j = score.to_json();
    j["version"] = major;

    std::stringstream ss;
    ss << score.id << score.game;
    auto hashstr = ss.str();
    auto hashfn = [](const int &sum, const char &ch) { return sum + ch; };
    auto hash = std::accumulate(hashstr.begin(), hashstr.end(), 0, hashfn);

    auto json = j.dump();

    std::stringstream req;
    req << "POST /scores?h=" << hash
        << " HTTP/1.0\r\n"
        << "Host: " << host << "\r\n"
        << "Content-Type: application/json\r\n"
        << "Content-Length: " << json.size() << "\r\n"
        << "Accept: */*\r\n\r\n"
        << json;

    tcp.send(req.str());
    std::string response = tcp.receive(4'096);
    return classifyResponse(response);
}

trippin::AddResult trippin::Transport::addLogEvent(const LogEvent &event) const {
    Tcp tcp(host, port);
    auto sock = tcp.get();
    if (sock == nullptr) {
        return AddResult::other;
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

    std::stringstream req;
    req << "POST /logs?h=" << hash
        << " HTTP/1.0\r\n"
        << "Host: " << host << "\r\n"
        << "Content-Type: application/json\r\n"
        << "Content-Length: " << json.size() << "\r\n"
        << "Accept: */*\r\n\r\n"
        << json;

    tcp.send(req.str());
    std::string response = tcp.receive(4'096);
    return classifyResponse(response);
}

trippin::AddResult trippin::Transport::classifyResponse(const std::string &response) {
    auto findFn = [&response](const char *str) {
        return response.find(str) != std::string::npos;
    };
    if (findFn(" 200 OK")) {
        return AddResult::success;
    }
    if (findFn(" 400 Bad Request") || findFn(" 404 Not Found")) {
        return AddResult::clientError;
    }
    return AddResult::other;
}

std::string trippin::Transport::getNotification() {
    Tcp tcp(host, port);
    auto sock = tcp.get();
    if (sock == nullptr) {
        return {};
    }

    std::stringstream req;
    req << "GET " << "/notifications/" << major << "/" << minor
        << " HTTP/1.0\r\n"
        << "Host: " << host << "\r\n"
        << "Accept: */*\r\n\r\n";

    tcp.send(req.str());
    std::string response = tcp.receive(1'024 * 4);

    auto f = response.find("\r\n\r\n");
    if (f == std::string::npos) {
        SDL_Log("response parse delimiter not found");
        return {};
    }

    if (response.find(" 200 OK") == std::string::npos) {
        SDL_Log("error response received from notification request");
        return {};
    }

    auto s = response.substr(f + 4);
    if (validNotification(s)) {
        return s;
    }

    SDL_Log("invalid characters in notification string");
    return {};
}

bool trippin::Transport::validNotification(std::string &notification) {
    for (auto &c: notification) {
        c = static_cast<char>(std::toupper(c)); // normalize notification string to uppercase
        if (!validNotificationChar(c)) {
            return false;
        }
    }
    return true;
}

bool trippin::Transport::validNotificationChar(char c) {
    return (c >= 'A' && c <= 'Z')
           || (c >= '0' && c <= '9')
           || c == '.'
           || c == ','
           || c == '!'
           || c == ' '
           || c == '\''
           || c == '\n';
}
