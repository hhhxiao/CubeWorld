#pragma once
#include "drawable_object.h"

struct ChunkPos;
struct BlockPos {
    int x{0};
    int y{0};
    int z{0};
    ChunkPos toChunkPos();
};

enum BlockType { air, grass, dirt, water, bedrock, invalid };
class Block {
   public:
   private:
    BlockPos pos{};
};
