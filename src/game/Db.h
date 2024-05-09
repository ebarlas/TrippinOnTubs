#ifndef TRIPPIN_DB_H
#define TRIPPIN_DB_H

#include <sstream>
#include <thread>
#include "SDL.h"
#include "nlohmann/json.hpp"
#include "sprite/Files.h"
#include "net/Channel.h"

namespace trippin {
    template<class T>
    class Db {
    public:
        struct Event {
            T val;
            bool add;

            bool operator==(const Event &e) const {
                return e.val == val;
            }
        };

        explicit Db(
                const char *name,
                int version,
                int maxChannelSize,
                std::function<void(const T &)> addCallback,
                std::function<bool(const T &)> dispatchFn) :
                name(name),
                fileName(makeFileName(name, version)),
                maxChannelSize(maxChannelSize),
                addCallback(addCallback),
                dispatchFn(dispatchFn) {
        }

        void start() {
            auto t1 = std::thread(&Db<T>::runJournalLoop, this);
            t1.detach();
            auto t2 = std::thread(&Db<T>::runDispatchLoop, this);
            t2.detach();
        }

        void add(const T &val) {
            inChannel.put({val, true});
        }

    private:
        const char *name;
        std::string fileName;
        Channel<Event> inChannel;
        Channel<T> outChannel;
        const int maxChannelSize;
        std::function<void(const T &)> addCallback;
        std::function<bool(const T &)> dispatchFn;

        static std::string makeFileName(const char *name, int version) {
            std::stringstream fileName;
            fileName << name << "_journal_" << version << ".txt";
            return fileName.str();
        }

        void load() {
            auto contents = readPrefFile(fileName.c_str());
            if (contents) {
                SDL_Log("initializing from journal file, file=%s", fileName.c_str());
                auto events = parseLines(*contents);
                addInitialEvents(events);
                SDL_Log("done initializing from journal file, file=%s", fileName.c_str());
            } else {
                SDL_Log("unable to initialize from journal file, file=%s", fileName.c_str());
            }
        }

        static std::vector<Event> parseLines(const std::string &lines) {
            std::istringstream iss(lines);
            std::string line;
            Event event;
            std::vector<Event> events;
            while (std::getline(iss, line)) {
                try {
                    auto j = nlohmann::json::parse(line);
                    j.get_to(event);
                    if (event.add) {
                        events.push_back(event);
                    } else {
                        events.erase(std::remove(events.begin(), events.end(), event), events.end());
                    }
                } catch (nlohmann::json::exception &e) {
                    SDL_Log("error occurred parsing json in journal, message=%s", e.what());
                    break;
                }
            }
            return events;
        }

        void addInitialEvents(const std::vector<Event> &events) {
            std::stringstream ss;
            for (auto &e: events) {
                ss << nlohmann::json(e).dump() << "\n";
            }
            writePrefFile(fileName.c_str(), ss.str());
            for (auto &e: events) {
                outChannel.put(e.val);
            }
        }

        void addNextEvent(Event &event) {
            appendPrefFile(fileName.c_str(), nlohmann::json(event).dump() + "\n");
            if (event.add) {
                outChannel.put(event.val);
            }
        }

        void runJournalLoop() {
            SDL_Log("started journal thread, file=%s", fileName.c_str());
            load();
            unsigned long iter = 0;
            while (true) {
                auto event = inChannel.take();
                if (!event) {
                    break;
                }
                addNextEvent(*event);
                if (event->add) {
                    addCallback(event->val);
                }
                iter++;
                auto *op = event->add ? "add" : "ack";
                SDL_Log("persisted event in journal, iter=%lu, op=%s, file=%s", iter, op, fileName.c_str());
            }
        }

        void runDispatchLoop() {
            SDL_Log("started dispatch thread, type=%s", name);
            unsigned long iter = 0;
            while (true) {
                auto val = outChannel.take();
                if (!val) {
                    break;
                }
                while (!dispatchFn(*val)) {
                    auto sz = outChannel.size();
                    if (sz >= maxChannelSize) {
                        SDL_Log("aborting dispatch due to full channel, iter=%lu, type=%s, size=%d", iter, name, sz);
                        break;
                    }
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                }
                inChannel.put({*val, false});
                iter++;
                SDL_Log("dispatched value, iter=%lu, type=%s", iter, name);
            }
        }
    };
}

#endif
