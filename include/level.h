#pragma once

#include <cstddef>
#include <queue>
#include <string>
#include <unordered_map>
#include "async_chunk_cache.h"
#include "block.h"
#include "chunk.h"
#include <cstdint>
#include <unordered_set>
#include "simple_shader.h"
#include "terrain_generator.h"
#include "lru.h"
#include "thread_pool.h"

class Level {
   public:
    Level();

    void draw(Shader *shader);
    void updatePlayerPos(const BlockPos &pos);

    inline ChunkPos getChunkPos() { return this->playerPos.toChunkPos(); }

    std::unordered_map<std::string, size_t> getChunkStats();

    ~Level();

   private:
    BlockPos playerPos;
    AsyncChunkCache chunk_cache_;
};
