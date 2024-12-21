#ifndef TIME_COUNTER
#define TIME_COUNTER

#include <cstddef>
#include <numeric>
#include <queue>
#include <chrono>
#include <ratio>

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
        time_queue_.push_back(duration.count());
        if (time_queue_.size() > max_size_) {
            time_queue_.pop_front();
        }
    }

    double mean() { return std::accumulate(time_queue_.begin(), time_queue_.end(), 0.0) / time_queue_.size(); }

   private:
    std::chrono::time_point<std::chrono::steady_clock> start_time_;

   private:
    size_t max_size_{20};
    std::deque<double> time_queue_;
};

#endif
