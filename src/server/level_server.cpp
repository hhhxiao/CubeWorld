#include "level_server.h"
#include <chrono>
#include <thread>
#include "config.h"
#include "utils.h"

LevelServer::LevelServer() {
    this->main_thread_ = new std::thread([this]() {
        using std::chrono::steady_clock;
        while (!stop_) {
            auto begin = steady_clock::now();
            this->tick();
            auto end = steady_clock::now();
            auto mspt = std::chrono::duration<double, std::milli>(end - begin);
            auto standard = std::chrono::milliseconds(Config::MAX_MSPT);
            if (mspt < standard) {
                std::this_thread::sleep_for(standard - mspt);
            }
        }
    });
}

void LevelServer::tick() {
    tick_++;
    LOGGER("tick = %zu", tick_);
}

void LevelServer::handleClientRequest(const glm::vec3 &cameraPos) {}

LevelServer::~LevelServer() {
    stop_ = true;
    main_thread_->join();
    delete main_thread_;
}
