#pragma once

#include <unordered_map>
#include "block.h"
#include "chunk.h"
#include <cstdint>
#include "simple_shader.h"
#include "terrain_generator.h"
#include "lru.h"
#include "texture.h"

class Level {
   public:
    Level();

    void draw(TexturePool *pool, Shader *shader);
    void updatePlayerPos(const BlockPos &pos);

   private:
    Chunk *generateNewChunk(const ChunkPos &pos);

    BlockPos playerPos;
    AbstractTerrainGenerator *generator{nullptr};
    std::unordered_map<uint64_t, Chunk *> chunks_;
    LRUPolicy<uint64_t> lru_;
};
