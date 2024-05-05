#pragma once

#include <cstddef>
#include <unordered_map>
#include "chunk.h"
#include "lru.h"
#include "terrain_generator.h"

#include "thread_pool.h"
#include <cstdint>
#include <mutex>
#include <unordered_set>

template <typename T>
class TaskBuffer {
   public:
    bool contains(const T &t) {
        bool exist{false};
        {
            std::lock_guard<std::mutex> lk(this->mu_);
            exist = this->buffer_.count(t) > 0;
        }
        return exist;
    }

    size_t size() {
        size_t sz = 0;
        {
            std::lock_guard<std::mutex> kl(this->mu_);
            sz = this->buffer_.size();
        }
        return sz;
    }

    void clear() {
        std::lock_guard<std::mutex> kl(this->mu_);
        this->buffer_.clear();
    }

    void add(const T &t) {
        std::lock_guard<std::mutex> kl(this->mu_);
        this->buffer_.insert(t);
    }

    void remove(const T &t) {
        std::lock_guard<std::mutex> kl(this->mu_);
        this->buffer_.erase(t);
    }

    std::mutex mu_;
    std::unordered_set<T> buffer_;
};

template <typename K, typename V>
class SimpleCoucurrentMap {
   public:
    void add(const K &k, const V &v) {
        std::lock_guard<std::mutex> lk(mu_);
        map_[k] = v;
    }

    bool get(const K &k, V &v) {
        bool res = false;
        std::lock_guard<std::mutex> lk(mu_);
        {
            auto it = this->map_.find(k);
            if (it != this->map_.end()) {
                res = true;
                v = it->second;
            }
        }
        return res;
    }

    void erase(const K &v) {
        std::lock_guard<std::mutex> lk(mu_);
        this->map_.erase(v);
    }

    size_t size() {
        size_t res;
        std::lock_guard<std::mutex> lk(mu_);
        { res = this->map_.size(); }
        return res;
    }

   private:
    std::unordered_map<K, V> map_;
    std::mutex mu_;
};

class AsyncChunkCache {
   public:
    AsyncChunkCache();

    Chunk *getChunk(const ChunkPos &pos);

    inline size_t ActiveChunkCount() { return chunk_cache_.size(); }
    inline size_t ChunkInTaskCount() const { return task_queue_.buffer_.size(); }

   private:
    SimpleCoucurrentMap<uint64_t, Chunk *> chunk_cache_;
    LRUPolicy<uint64_t> lru_;
    AbstractTerrainGenerator *generator_{nullptr};
    TaskBuffer<uint64_t> task_queue_;
    ThreadPool *pool_;
};