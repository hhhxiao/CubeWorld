#include "async_chunk_cache.h"
#include "chunk.h"
#include "config.h"
#include "terrain_generator.h"
#include "thread_pool.h"

AsyncChunkCache::AsyncChunkCache() {
    this->generator_ = new FlatTerrainGenerator();
    this->pool_ = new ThreadPool(2);
}

Chunk* AsyncChunkCache::getChunk(const ChunkPos& pos) {
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

    // 因为析构要释放mesh，所以需要在这里析构w
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
