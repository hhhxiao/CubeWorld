#include "chunk_builder.h"
#include <chrono>
#include <cstddef>
#include <thread>
#include <vector>
#include "block.h"
#include "chunk.h"
#include "config.h"
#include "position.h"
#include "terrain_generator.h"
#include "thread_pool.h"
#include "utils.h"

ChunkBuilder::ChunkBuilder() {
    this->terrain_generator_ = new PerlinTerrainGeneratror(20);
    this->pool_ = new ThreadPool(Config::CHUNK_GEN_THREAD_NUM);
    // for debug
    for (int i = 0; i < Config::CHUNK_GEN_THREAD_NUM; i++) {
        this->pool_->enqueue([]() {
            loguru::set_thread_name("Chunk Generation");
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        });
    }
}

void ChunkBuilder::tick(tick_t ts) {
    // clean old chunks
    std::vector<ChunkPos> deads;
    for (auto& chunk : this->chunks_) {
        if (chunk.second->isDead(ts)) {
            deads.emplace_back(chunk.first);
        }
    }
    for (auto& d : deads) {
        auto it = chunks_.find(d);
        CHECK_F(it != chunks_.end(), "Chunk data error");
        delete it->second;
        LD("Remove dead chunk (%d %d)", d.x, d.z);
        chunks_.erase(it);
    }
}

LevelChunk* ChunkBuilder::requestChunk(const ChunkPos& pos) {
    auto it = chunks_.find(pos);
    if (it != chunks_.end()) {
        return it->second;
    }
    if (!task_queue_.contains(pos)) {
        task_queue_.insert(pos);
        this->pool_->enqueue([pos, this]() {
            auto* chunk = new LevelChunk(pos);
            terrain_generator_->fill(chunk);
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            LD("Generate Chunk (%d %d)", pos.x, pos.z);
            if (chunks_.contains(pos)) {
                LW("Duplicated chunk generated (%d %d)", pos.x, pos.z);
            }
            chunks_[pos] = chunk;
            task_queue_.erase(pos);
        });
    }
    return nullptr;
}

ChunkBuilder::~ChunkBuilder() {
    delete pool_;
    for (auto& kv : chunks_) {
        delete kv.second;
    }
}