#pragma once

#include "chunk.h"
#include "lru.h"
#include "position.h"
#include "terrain_generator.h"
#include "thread_pool.h"
#include "utils.h"
#include <cstdint>
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
    auto &allLiveChunks() { return chunks_; }

   private:
    LRUPolicy<uint64_t> lru_;  // For clean chunks
    pfhmap<ChunkPos, LevelChunk *> chunks_;
    // task buffer
    phmap::parallel_flat_hash_set<ChunkPos> task_queue_;
    ThreadPool *pool_;
};