#pragma once

#include "chunk.h"
#include "lru.h"
#include "position.h"
#include "terrain_generator.h"
#include "thread_pool.h"
#include "utils.h"
#include <cstdint>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include "utils.h"
#include <moodycamel/concurrentqueue.h>

class ChunkBuilder {
   public:
    ChunkBuilder();
    ~ChunkBuilder();

   public:
    LevelChunk *requestChunk(const ChunkPos &pos);

    AbstractTerrainGenerator *terrain_generator_{nullptr};
    void tick(tick_t ts);
    const auto &allLiveChunks() const { return chunks_; }

   private:
    LRUPolicy<uint64_t> lru_;  // For clean chunks
    std::unordered_map<ChunkPos, LevelChunk *> chunks_;
    // task buffer
    std::unordered_set<ChunkPos> task_queue_;
    std::mutex lock_;
    progschj::ThreadPool *pool_;
};
