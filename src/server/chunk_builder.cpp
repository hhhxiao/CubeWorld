#include "chunk_builder.h"
#include "block.h"
#include "chunk.h"
#include "config.h"
#include "terrain_generator.h"
#include "thread_pool.h"

AsyncChunkCache::AsyncChunkCache() {
    this->generator_ = new PerlinTerrainGeneratror(100);
    this->pool_ = new ThreadPool(2);
}

Chunk* AsyncChunkCache::getChunk(const ChunkPos& pos) {
    auto hash = pos.hash();
    Chunk* chunk{nullptr};
    if (this->chunk_cache_.get(hash, chunk)) {
        return nullptr;
    }
    return chunk;
}

Chunk* AsyncChunkCache::postGetChunk(const ChunkPos& pos) {
    auto hash = pos.hash();

    Chunk* chunk{nullptr};
    if (this->chunk_cache_.get(hash, chunk)) {
        lru_.promote(hash);
        return chunk;
    }

    // post new taks
    if (!this->task_queue_.contains(hash)) {
        this->pool_->enqueue([this, pos, hash]() {
            auto* chunk = new Chunk(pos, this->generator_);
            this->chunk_cache_.add(hash, chunk);
            lru_.insert(hash);
            this->task_queue_.remove(hash);
        });
    }

    // 因为析构要释放mesh，所以需要在这里析构
    while (this->chunk_cache_.size() > Config::chunk_cache_size) {
        auto old = this->lru_.oldest();
        lru_.evict(old);
        Chunk* to_deleted{nullptr};
        if (this->chunk_cache_.get(old, to_deleted)) {
            this->chunk_cache_.erase(old);
            delete chunk;
        }
    }

    return nullptr;
}

void AsyncChunkCache::genBlockFaces(std::vector<BlockFaceInfo>& info) {
    // /auto& data = this->chunk_cache_.data();
}

ChunkBuilder::ChunkBuilder() { this->pool_ = new ThreadPool(4); }

ChunkBuilder::~ChunkBuilder() {}