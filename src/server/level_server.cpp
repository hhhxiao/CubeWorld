#include "level_server.h"
#include <chrono>
#include <thread>
#include "bridge.h"
#include "config.h"
#include "player.h"
#include "utils.h"

LevelServer::LevelServer(DataBridge* bridge) : bridge_(bridge) { this->player_ = new Player(); }

void LevelServer::start() {
    LI("Server started");
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
}

void LevelServer::tick() {
    tick_++;
    syncRead();
    syncWrite();
}

void LevelServer::syncRead() {
    auto& buffer = bridge_->clientBuffer();
    if (!buffer.dirty()) return;
    player_->setPos(buffer.camera_position);
    buffer.cleanDirty();
}

void LevelServer::syncWrite() {
    auto& buffer = bridge_->serverBuffer();
    buffer.beginWrite();
    LD("Write buffer(version: %zu)", buffer.version());
    buffer.endWrite();
}

void LevelServer::stop() {
    LI("Stoping server ...");
    stop_ = true;
}

LevelServer::~LevelServer() { LI("Server stopped"); }