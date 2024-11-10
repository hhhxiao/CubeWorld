#include "level_server.h"
#include <chrono>
#include <thread>
#include "bridge.h"
#include "chunk_builder.h"
#include "config.h"
#include "glm/detail/type_vec.hpp"
#include "player.h"
#include "position.h"
#include "utils.h"

LevelServer::LevelServer(DataBridge* bridge) : bridge_(bridge) {
    this->player_ = new Player();
    this->chunk_builder_ = new ChunkBuilder();
}

void LevelServer::start() {
    LI("Server start");
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
    tickChunks();
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
    // send chunks
    // delete old chunks
    // add new chunks
    // for (auto& chunk : buffer.chunks) {
    // }
    //  LD("Write buffer(version: %zu)", buffer.version());
    buffer.endWrite();
}

void LevelServer::tickChunks() {
    auto R = Config::load_radius;
    auto cp = BlockPos::fromVec3(player_->getPos()).toChunkPos();
    for (auto i = -R; i <= R; i++) {
        for (auto j = -R; j <= R; j++) {
            glm::vec3(1, 1, 1);
            auto pos = ChunkPos(cp.x + i, cp.z + j);
            auto* chunk = chunk_builder_->requestChunk(pos);
            if (chunk) chunk->tick(tick_);
        }
    }
    chunk_builder_->tick(tick_);
}

void LevelServer::stop() {
    LI("Stoping server ...");
    stop_ = true;
}

LevelServer::~LevelServer() {
    delete player_;
    delete chunk_builder_;
    LI("Server stopped");
}
