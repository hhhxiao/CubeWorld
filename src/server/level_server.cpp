#include "level_server.h"
#include <chrono>
#include <thread>
#include "bridge.h"
#include "chunk.h"
#include "chunk_builder.h"
#include "config.h"
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
    mspt_timer_.start();
    tick_++;
    syncRead();
    syncWrite();
    tickChunks();
    mspt_timer_.end();
}

void LevelServer::syncRead() {
    auto& buffer = bridge_->clientBuffer();
    if (!buffer.beginRead()) return;
    if (buffer.dirty()) {
        player_->setPos(buffer.camera_position);
        buffer.cleanDirty();
    }
    buffer.endRead();
}

void LevelServer::syncWrite() {
    auto cp = player_->getChunkPos();
    auto& buffer = bridge_->serverBuffer();

    buffer.beginWrite();
    for (auto it = buffer.chunks.begin(); it != buffer.chunks.end();) {
        if (cp.dis2(it->first) > Config::LOAD_RADIUS * Config::LOAD_RADIUS) {
            it = buffer.chunks.erase(it);
        } else {
            ++it;
        }
    }

    auto& liveChunks = chunk_builder_->allLiveChunks();
    for (auto& [pos, c] : liveChunks) {
        if (cp.dis2(pos) > Config::LOAD_RADIUS * Config::LOAD_RADIUS) continue;
        if (!buffer.chunks.count(pos)) {
            buffer.chunks.emplace(pos, *c);
        }
    }

    buffer.player_position = player_->getPos();
    buffer.mspt = mspt_timer_.mean();
    buffer.chunk_cache_size = liveChunks.size();
    buffer.endWrite();
}

void LevelServer::tickChunks() {
    const auto R = Config::LOAD_RADIUS;
    const auto cp = BlockPos::fromVec3(player_->getPos()).toChunkPos();
    for (auto i = -R; i <= R; i++) {
        for (auto j = -R; j <= R; j++) {
            auto pos = ChunkPos(cp.x + i, cp.z + j);
            if (auto* chunk = chunk_builder_->requestChunk(pos)) chunk->tick(tick_);
        }
    }
    chunk_builder_->tick(tick_);
}

void LevelServer::stop() {
    LI("Stopping server ...");
    stop_ = true;
}

LevelServer::~LevelServer() {
    delete player_;
    delete chunk_builder_;
    LI("Server stopped");
}