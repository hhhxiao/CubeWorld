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

// AsyncChunkCache::AsyncChunkCache() {
//     this->generator_ = new PerlinTerrainGeneratror(100);
//     this->pool_ = new ThreadPool(2);
// }

// Chunk* AsyncChunkCache::getChunk(const ChunkPos& pos) {
//     auto hash = pos.hash();
//     Chunk* chunk{nullptr};
//     if (this->chunk_cache_.get(hash, chunk)) {
//         return nullptr;
//     }
//     return chunk;
// }

// Chunk* AsyncChunkCache::postGetChunk(const ChunkPos& pos) {
//     auto hash = pos.hash();

//     Chunk* chunk{nullptr};
//     if (this->chunk_cache_.get(hash, chunk)) {
//         lru_.promote(hash);
//         return chunk;
//     }

//     // post new taks
//     if (!this->task_queue_.contains(hash)) {
//         this->pool_->enqueue([this, pos, hash]() {
//             auto* chunk = new Chunk(pos, this->generator_);
//             this->chunk_cache_.add(hash, chunk);
//             lru_.insert(hash);
//             this->task_queue_.remove(hash);
//         });
//     }

//     // 因为析构要释放mesh，所以需要在这里析构
//     while (this->chunk_cache_.size() > Config::chunk_cache_size) {
//         auto old = this->lru_.oldest();
//         lru_.evict(old);
//         Chunk* to_deleted{nullptr};
//         if (this->chunk_cache_.get(old, to_deleted)) {
//             this->chunk_cache_.erase(old);
//             delete chunk;
//         }
//     }

//     return nullptr;
// }

// void AsyncChunkCache::genBlockFaces(std::vector<BlockFaceInfo>& info) {
//     // /auto& data = this->chunk_cache_.data();
// }
#include "utils.h"

ChunkBuilder::ChunkBuilder() {
    this->terrain_generator_ = new FlatTerrainGenerator();
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
    LD("Chunk cache size = %zu", chunks_.size());
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
