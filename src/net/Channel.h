#ifndef TRIPPIN_CHANNEL_H
#define TRIPPIN_CHANNEL_H

#include <queue>
#include <optional>
#include <mutex>
#include <condition_variable>

namespace trippin {
    template<class T>
    class Channel {
    public:
        std::optional<T> take() {
            std::unique_lock lock(mutex);
            cv.wait(lock, [this]{ return closed || !queue.empty(); });
            if (closed) {
                return {};
            }
            auto e = queue.front();
            queue.pop();
            return e;
        }
        bool put(T elem) {
            {
                std::lock_guard lock(mutex);
                if (closed) {
                    return false;
                }
                queue.push(elem);
            }
            cv.notify_one();
            return true;
        }
        void close() {
            {
                std::unique_lock lk(mutex);
                closed = true;
            }
            cv.notify_all();
        }
    private:
        std::queue<T> queue;
        std::mutex mutex;
        std::condition_variable cv;
        bool closed{};
    };
}

#endif
