#ifndef TRIPPIN_CLOCKSTATS_H
#define TRIPPIN_CLOCKSTATS_H

namespace trippin {
    class ClockStats {
    public:
        explicit ClockStats(bool skipFirst = true)
                : skipFirst(skipFirst), first(false), min(-1), max(-1), count(0), sum(0) {}

        void add(int tps) {
            if (!first) {
                first = true;
                if (skipFirst) {
                    return;
                }
            }
            if (min < 0 || tps < min) {
                min = tps;
            }
            if (max < 0 || tps > max) {
                max = tps;
            }
            sum += tps;
            count++;
        }

        int getMin() const {
            return min;
        }

        int getMax() const {
            return max;
        }

        int getAvg() const {
            return count > 0 ? sum / count : -1;
        }

    private:
        const bool skipFirst;
        bool first;
        int min;
        int max;
        int count;
        int sum;
    };
}

#endif
