#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include "catch2/catch.hpp"
#include "net/Channel.h"

void takeLoop(trippin::Channel<int> *channel, std::vector<int> *vals, std::mutex *mutex) {
    while (true) {
        auto opt = channel->take();
        if (!opt) {
            return;
        }
        {
            std::lock_guard<std::mutex> lg(*mutex);
            vals->push_back(*opt);
        }
    }
}

TEST_CASE("Channel Lifecycle", "[channel]")
{
    trippin::Channel<int> channel;
    std::vector<int> vals;
    std::mutex mutex;
    std::thread t(takeLoop, &channel, &vals, &mutex);
    int count = 5;
    for (int i = 0; i < count; i++) {
        channel.put(i);
    }
    auto now = []{
        return std::chrono::steady_clock::now();
    };
    auto done = [&mutex, &vals, count]() {
        std::lock_guard<std::mutex> lg(mutex);
        return vals.size() == count;
    };
    auto deadline = now() + std::chrono::seconds(3);
    while (!done() && now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    channel.close();
    t.join();
    REQUIRE(done());
}