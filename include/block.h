#pragma once
#include "drawable_object.h"

struct BlockPos {
    int x{0};
    int y{0};
    int z{0};
};

enum BlockType { air, grass, dirt, bedrock, invalid };
class Block {
   public:
   private:
    BlockPos pos{};
};
