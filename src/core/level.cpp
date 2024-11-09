#include "level.h"
#include <chrono>
#include <thread>
#include "config.h"
#include "utils.h"

Level::Level() {
    this->main_thread_ = new std::thread([this]() {
        using std::chrono::steady_clock;
        while (!stop_) {
            auto begin = steady_clock::now();
            this->serverTick();
            auto end = steady_clock::now();
            auto mspt = std::chrono::duration<double, std::milli>(end - begin);
            auto standard = std::chrono::milliseconds(Config::MAX_MSPT);
            if (mspt < standard) {
                std::this_thread::sleep_for(standard - mspt);
            }
        }
    });
}

void Level::tick() {}

void Level::serverTick() {
    tick_++;
    LOGGER("tick = %zu", tick_);
}

Level::~Level() {
    stop_ = true;
    main_thread_->join();
    delete main_thread_;
}
