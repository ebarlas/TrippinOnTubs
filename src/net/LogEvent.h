#ifndef TRIPPIN_LOG_EVENT_H
#define TRIPPIN_LOG_EVENT_H

#include <string>
#include "nlohmann/json.hpp"

namespace trippin {
    struct LogEvent {
        std::string id;
        int index;
        std::string message;

        bool operator==(const LogEvent &e) const {
            return id == e.id && index == e.index;
        }
    };

    inline void to_json(nlohmann::json& j, const LogEvent& e) {
        j["id"] = e.id;
        j["index"] = e.index;
        j["message"] = e.message;
    }

    inline void from_json(const nlohmann::json &j, trippin::LogEvent &e) {
        j.at("id").get_to(e.id);
        j.at("index").get_to(e.index);
        j.at("message").get_to(e.message);
    }
}

#endif
