#ifndef TIME_COUNTER
#define TIME_COUNTER

#include <algorithm>
#include <cstddef>
#include <numeric>
#include <deque>
#include <chrono>
#include <ratio>
#include "utils.h"

class PeriodTimer {
   public:
    PeriodTimer(int size) : max_size_(size) {}
    PeriodTimer() = default;

    void start() {
        using std::chrono::steady_clock;
        start_time_ = steady_clock::now();
    }
    void end() {
        using std::chrono::steady_clock;
        auto end_time = steady_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end_time - start_time_);
        push(duration.count());
    }

    void push(double val) {
        time_queue_.push_back(val);
        if (time_queue_.size() > max_size_) {
            time_queue_.pop_front();
        }
    }

    double mean() { return std::accumulate(time_queue_.begin(), time_queue_.end(), 0.0) / time_queue_.size(); }

    double low(int percent) {
        percent = std::clamp(percent, 1, 100);
        auto data = time_queue_;
        std::ranges::sort(data);
        auto cnt = static_cast<size_t>(static_cast<double>((max_size_ * percent)) / 100.0);
        if (cnt <= 0 || time_queue_.size() < cnt) return mean();
        return std::accumulate(data.begin(), data.begin() + cnt, 0.0) / cnt;
    }

   private:
    std::chrono::time_point<std::chrono::steady_clock> start_time_;

   private:
    size_t max_size_{20};
    std::deque<double> time_queue_;
};

#endif
